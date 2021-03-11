#include "usb_impl.h"

#include "src/logging.h"
#include "src/util/util.h"

namespace threeboard {
namespace usb {

constexpr uint8_t kFrameTimeout = 50;

UsbImpl::UsbImpl(native::Native *native, ErrorHandlerDelegate *error_handler)
    : native_(native), error_handler_(error_handler) {
  native_->SetUsbInterruptHandlerDelegate(this);
  // There's no reason to expose RequestHandler outside usb/internal, but we
  // also need to be able to inject a mock. Instead of exposing it, we compose
  // it here and allow overwriting the pointer for tests using a friend
  // declaration.
  static RequestHandler handler(native_);
  request_handler_ = &handler;
}

bool UsbImpl::Setup() {
  // Enable the USB pad regulator (which uses the external 1uF UCap).
  native_->SetUHWCON(1 << native::UVREGE);
  // Enable USB and freeze the clock.
  native_->SetUSBCON((1 << native::USBE) | (1 << native::FRZCLK));
  // PLL Control and Status Register. Configure the prescaler for the 16MHz
  // clock, and enable the PLL.
  native_->SetPLLCSR((1 << native::PINDIV) | (1 << native::PLLE));
  // Busy loop to wait for the PLL to lock to the 16MHz reference clock.
  uint16_t iterations = 0;
  while (!(native_->GetPLLCSR() & (1 << native::PLOCK))) {
    if (iterations == UINT16_MAX) {
      LOG_ONCE("USB Setup error (likely fatal)");
      return false;
    }
    iterations += 1;
  }

  // Enable USB and the VBUS pad.
  native_->SetUSBCON((1 << native::USBE) | (1 << native::OTGPADE));
  // Configure USB general interrupts (handled by the USB_GEN_vect routine). We
  // want to interrupt on start of frame (SOFE), and also on end of reset
  // (EORSTE).
  native_->SetUDIEN((1 << native::EORSTE) | (1 << native::SOFE));
  // Connect internal pull-up attach resistor. This must be the final step in
  // the setup process because it indicates that the device is now ready.
  native_->SetUDCON(native_->GetUDCON() & ~(1 << native::DETACH));
  return true;
}

bool UsbImpl::HasConfigured() { return hid_state_.configuration; }

bool UsbImpl::SendKeypress(const uint8_t key, const uint8_t mod) {
  hid_state_.modifier_keys = mod;
  // Currently we only support sending a single key at a time, even though this
  // USB implementation supports the full 6 keys. Functionality of the
  // threeboard may change in future to send multiple keys.
  hid_state_.keyboard_keys[0] = key;
  RETURN_IF_ERROR(SendKeypress());
  hid_state_.modifier_keys = 0;
  hid_state_.keyboard_keys[0] = 0;
  RETURN_IF_ERROR(SendKeypress());
  return true;
}

void UsbImpl::HandleGeneralInterrupt() {
  uint8_t device_interrupt = native_->GetUDINT();
  native_->SetUDINT(0);
  // Detect end of reset interrupt, and configure Endpoint 0.
  if (device_interrupt & (1 << native::EORSTI)) {
    // Switch to Endpoint 0.
    native_->SetUENUM(0);
    // Re-enable the endpoint after this USB reset.
    native_->SetUECONX(1 << native::EPEN);
    // Set Endpoint 0's type as a control endpoint.
    native_->SetUECFG0X(0);
    // Set Endpoint 0's size as 32 bytes, single bank allocated.
    native_->SetUECFG1X(kEndpoint32ByteBank | kEndpointSingleBank);
    // Configure an endpoint interrupt when RXSTPI is sent (i.e. when the
    // current bank contains a new valid SETUP packet).
    native_->SetUEIENX(1 << native::RXSTPE);
  }

  // SOFI (start of frame interrupt) will fire every 1ms on our full speed bus.
  // We use it to time the HID reporting frequency based on the idle rate once
  // the device has been configured. Some hosts may disable this by setting
  // idle_config to 0.
  if ((device_interrupt & (1 << native::SOFI)) &&
      !(native_->GetUDMFN() & (1 << native::FNCERR)) &&
      hid_state_.configuration && hid_state_.idle_config) {
    native_->SetUENUM(kKeyboardEndpoint);
    // Check we're allowed to write out to USB FIFO.
    if (native_->GetUEINTX() & (1 << native::RWAL)) {
      hid_state_.idle_count++;
      if (hid_state_.idle_count == hid_state_.idle_config) {
        // TODO: we should check if there's something in the IN buffer already
        // before sending zeroes, otherwise we may miss keystrokes.
        SendHidState();
      }
    }
  }
}

void UsbImpl::HandleEndpointInterrupt() {
  // Immediately parse incoming data into a SETUP packet. If there's an issue
  // with the interrupt type it'll be handled afterwards.
  SetupPacket packet = SetupPacket::ParseFromUsbEndpoint(native_);
  // Backup and clear interrupt bits to handshake the interrupt.
  uint8_t interrupt = native_->GetUEINTX();
  native_->SetUEINTX(
      native_->GetUEINTX() &
      ~((1 << native::RXSTPI) | (1 << native::RXOUTI) | (1 << native::TXINI)));

  // This interrupt handler is only used for responding to USB SETUP packets. If
  // the interrupt has triggered for any other reason, we should reply with
  // STALL.
  if (!(interrupt & (1 << native::RXSTPI))) {
    native_->SetUECONX((1 << native::STALLRQ) | (1 << native::EPEN));
    return;
  }

  // Call the appropriate device handlers for device requests.
  if (packet.bRequest == Request::GET_STATUS) {
    request_handler_->HandleGetStatus();
  }
  if (packet.bRequest == Request::SET_ADDRESS) {
    request_handler_->HandleSetAddress(packet);
  }
  if (packet.bRequest == Request::GET_DESCRIPTOR) {
    request_handler_->HandleGetDescriptor(packet);
  }
  if (packet.bRequest == Request::GET_CONFIGURATION &&
      packet.bmRequestType.GetDirection() ==
          RequestType::Direction::DEVICE_TO_HOST) {
    request_handler_->HandleGetConfiguration(hid_state_);
  }
  if (packet.bRequest == Request::SET_CONFIGURATION &&
      packet.bmRequestType.GetDirection() ==
          RequestType::Direction::HOST_TO_DEVICE) {
    request_handler_->HandleSetConfiguration(packet, &hid_state_);
  }

  // Call the appropriate HID handlers for HID requests.
  if (packet.wIndex == kKeyboardInterface &&
      packet.bmRequestType.GetType() == RequestType::Type::CLASS &&
      packet.bmRequestType.GetRecipient() ==
          RequestType::Recipient::INTERFACE) {
    if (packet.bmRequestType.GetDirection() ==
        RequestType::Direction::DEVICE_TO_HOST) {
      if (packet.bRequest == Request::HID_GET_REPORT) {
        request_handler_->HandleGetReport(hid_state_);
      }
      if (packet.bRequest == Request::HID_GET_IDLE) {
        request_handler_->HandleGetIdle(hid_state_);
      }
      if (packet.bRequest == Request::HID_GET_PROTOCOL) {
        request_handler_->HandleGetProtocol(hid_state_);
      }
    }
    if (packet.bmRequestType.GetDirection() ==
        RequestType::Direction::HOST_TO_DEVICE) {
      if (packet.bRequest == Request::HID_SET_IDLE) {
        request_handler_->HandleSetIdle(packet, &hid_state_);
      }
      if (packet.bRequest == Request::HID_SET_PROTOCOL) {
        request_handler_->HandleSetProtocol(packet, &hid_state_);
      }
    }
  }
}

bool UsbImpl::SendKeypress() {
  uint8_t intr_state = native_->GetSREG();
  native_->DisableInterrupts();
  uint8_t initial_frame_num = native_->GetUDFNUML();
  while (true) {
    native_->SetUENUM(kKeyboardEndpoint);
    native_->EnableInterrupts();
    // Check if we're allowed to push data into the FIFO. If we are, we can
    // immediately break and begin transmitting.
    if (native_->GetUEINTX() & (1 << native::RWAL)) {
      break;
    }
    native_->SetSREG(intr_state);
    // Ensure the device is still configured.
    RETURN_IF_ERROR(hid_state_.configuration);
    // Only continue polling RWAL for 50 frames (50ms on our full-speed bus).
    if ((native_->GetUDFNUML() - initial_frame_num) >= kFrameTimeout) {
      return false;
    }
    intr_state = native_->GetSREG();
    native_->DisableInterrupts();
  }

  SendHidState();
  native_->SetSREG(intr_state);
  return true;
}

// Send the state of the HID device to the bus.
void UsbImpl::SendHidState() {
  hid_state_.idle_count = 0;
  native_->SetUEDATX(hid_state_.modifier_keys);
  native_->SetUEDATX(0);
  for (uint8_t i = 0; i < 6; i++) {
    native_->SetUEDATX(hid_state_.keyboard_keys[i]);
  }
  // Reset UEINTX after send
  native_->SetUEINTX((1 << native::RWAL) | (1 << native::NAKOUTI) |
                     (1 << native::RXSTPI) | (1 << native::STALLEDI));
}

}  // namespace usb
}  // namespace threeboard
