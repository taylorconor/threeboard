// This USB implementation is influenced by the LUFA project
// (https://github.com/abcminiuser/lufa), and by the Atreus firmware
// (https://github.com/technomancy/atreus-firmware).
// It explicitly does not support the ENDPOINT_HALT feature, since it's rarely
// used and shouldn't affect functionality at all.

#include "usb_impl.h"

#include "usb_common.h"
#include "usb_descriptors.h"
#include "usb_handlers.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

namespace threeboard {
namespace native {
namespace {

// TODO: move to header
static UsbHidState hid_state;

// Misc functions to wait for ready and send/receive packets
static inline void AwaitTransmitterReady() {
  while (!(UEINTX & (1 << TXINI)))
    ;
}
static inline void AwaitReceiverReady() {
  while (!(UEINTX & (1 << RXOUTI)))
    ;
}
static inline void HandshakeTransmitterInterrupt() { UEINTX = ~(1 << TXINI); }
static inline void HandshakeReceiverInterrupt() { UEINTX = ~(1 << RXOUTI); }

// Send the state of the HID device to the bus.
void SendHidState() {
  hid_state.idle_count = 0;
  UEDATX = hid_state.modifier_keys;
  UEDATX = 0;
  for (uint8_t i = 0; i < 6; i++) {
    UEDATX = hid_state.keyboard_keys[i];
  }
  UEINTX = 0x3A;
}

// USB General Interrupt request routine.
ISR(USB_GEN_vect) {
  uint8_t device_interrupt = UDINT;
  UDINT = 0;
  // Detect end of reset interrupt, and configure Endpoint 0.
  if (device_interrupt & (1 << EORSTI)) {
    // Switch to Endpoint 0.
    UENUM = 0;
    // Re-enable the endpoint after this USB reset.
    UECONX = 1 << EPEN;
    // Set Endpoint 0's type as a control endpoint.
    UECFG0X = 0;
    // Set Endpoint 0's size as 32 bytes, single bank allocated.
    UECFG1X = kEndpoint32ByteBank | kEndpointSingleBank;
    // Configure an endpoint interrupt when RXSTPI is sent (i.e. when the
    // current bank contains a new valid SETUP packet).
    UEIENX = (1 << RXSTPE);
  }

  // SOFI (start of frame interrupt) will fire every 1ms on our full speed bus.
  // We use it to time the HID reporting frequency based on the idle rate once
  // the device has been configured. Some hosts may disable this by setting
  // idle_config to 0.
  if ((device_interrupt & (1 << SOFI)) && !(UDMFN & (1 << FNCERR)) &&
      hid_state.configuration && hid_state.idle_config) {
    UENUM = kKeyboardEndpoint;
    // Check we're allowed to write out to USB FIFO.
    if (UEINTX & (1 << RWAL)) {
      hid_state.idle_count++;
      if (hid_state.idle_count == hid_state.idle_config) {
        // TODO: we should check if there's something in the IN buffer already
        // before sending zeroes, otherwise we may miss keystrokes.
        SendHidState();
      }
    }
  }
}

// USB Endpoint Interrupt request routine.
ISR(USB_COM_vect) {
  // Immediately parse incoming data into a SETUP packet. If there's an issue
  // with the interrupt type it'll be handled afterwards.
  SetupPacket packet = SetupPacket::ParseFromUsbEndpoint();
  // Backup and clear interrupt bits to handshake the interrupt.
  uint8_t interrupt = UEINTX;
  UEINTX &= ~((1 << RXSTPI) | (1 << RXOUTI) | (1 << TXINI));

  // This interrupt handler is only used for responding to USB SETUP packets. If
  // the interrupt has triggered for any other reason, we should reply with
  // STALL.
  if (!(interrupt & (1 << RXSTPI))) {
    UECONX = (1 << STALLRQ) | (1 << EPEN);
    return;
  }

  // Call the appropriate device handlers for device requests.
  if (packet.bRequest == Request::GET_STATUS) {
    device_handler::HandleGetStatus();
  }
  if (packet.bRequest == Request::SET_ADDRESS) {
    device_handler::HandleSetAddress(packet);
  }
  if (packet.bRequest == Request::GET_DESCRIPTOR) {
    device_handler::HandleGetDescriptor(packet);
  }
  if (packet.bRequest == Request::GET_CONFIGURATION &&
      packet.bmRequestType.GetDirection() ==
          RequestType::Direction::DEVICE_TO_HOST) {
    device_handler::HandleGetConfiguration(hid_state);
  }
  if (packet.bRequest == Request::SET_CONFIGURATION &&
      packet.bmRequestType.GetDirection() ==
          RequestType::Direction::HOST_TO_DEVICE) {
    device_handler::HandleSetConfiguration(packet, &hid_state);
  }

  // Call the appropriate HID handlers for HID requests.
  if (packet.wIndex == kKeyboardInterface &&
      packet.bmRequestType.GetType() == RequestType::Type::CLASS &&
      packet.bmRequestType.GetRecipient() ==
          RequestType::Recipient::INTERFACE) {
    if (packet.bmRequestType.GetDirection() ==
        RequestType::Direction::DEVICE_TO_HOST) {
      if (packet.bRequest == Request::HID_GET_REPORT) {
        hid_handler::HandleGetReport(hid_state);
      }
      if (packet.bRequest == Request::HID_GET_IDLE) {
        hid_handler::HandleGetIdle(hid_state);
      }
      if (packet.bRequest == Request::HID_GET_PROTOCOL) {
        hid_handler::HandleGetProtocol(hid_state);
      }
    }
    if (packet.bmRequestType.GetDirection() ==
        RequestType::Direction::HOST_TO_DEVICE) {
      if (packet.bRequest == Request::HID_SET_IDLE) {
        hid_handler::HandleSetIdle(packet, &hid_state);
      }
      if (packet.bRequest == Request::HID_SET_PROTOCOL) {
        hid_handler::HandleSetProtocol(packet, &hid_state);
      }
    }
  }
}

// send the contents of keyboard_keys and keyboard_modifier_keys
int8_t InternalSendKeypress() {
  uint8_t intr_state = SREG;
  cli();
  uint8_t timeout = UDFNUML + 50;
  while (1) {
    UENUM = kKeyboardEndpoint;
    sei();
    // Check if we're allowed to push data into the FIFO. If we are, we can
    // immediately break and begin transmitting.
    if (UEINTX & (1 << RWAL)) {
      break;
    }
    SREG = intr_state;
    // Ensure the device is still configured.
    if (!hid_state.configuration) {
      return -1;
    }
    // Only continue polling RWAL for 50 frames (50ms on our full-speed bus)
    if (UDFNUML >= timeout) {
      return -1;
    }
    intr_state = SREG;
    cli();
  }

  SendHidState();
  SREG = intr_state;
  return 0;
}
} // namespace

void UsbImpl::Setup() {
  // Enable the USB pad regulator (which uses the external 1uF UCap).
  UHWCON = 1 << UVREGE;
  // Enable USB and freeze the clock.
  USBCON = (1 << USBE) | (1 << FRZCLK);
  // PLL Control and Status Register. Configure the prescaler for the 16MHz
  // clock, and enable the PLL.
  PLLCSR = (1 << PINDIV) | (1 << PLLE);
  // Busy loop to wait for the PLL to lock to the 16MHz reference clock.
  while (!(PLLCSR & (1 << PLOCK)))
    ;
  // Enable USB and the VBUS pad.
  USBCON = (1 << USBE) | (1 << OTGPADE);
  // Connect internal pull-up attach resistor.
  UDCON &= ~(1 << DETACH);
  // Configure USB general interrupts (handled by the USB_GEN_vect routine). We
  // want to interrupt on start of frame (SOFE), and also on end of reset
  // (EORSTE).
  UDIEN = (1 << EORSTE) | (1 << SOFE);
}

void UsbImpl::SendKeypress(const uint8_t key, const uint8_t mod) {
  hid_state.modifier_keys = mod;
  // Currently we only support sending a single key at a time, even though this
  // USB implementation supports the full 6 keys. Functionality of the
  // threeboard may change in future to send multiple keys.
  hid_state.keyboard_keys[0] = key;
  // TODO: capture errors
  InternalSendKeypress();
  hid_state.modifier_keys = 0;
  hid_state.keyboard_keys[0] = 0;
  // TODO: capture errors
  InternalSendKeypress();
}
} // namespace native
} // namespace threeboard
