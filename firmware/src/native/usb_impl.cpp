// This USB implementation is heavily influenced by the LUFA project
// (https://github.com/abcminiuser/lufa), and by the Atreus firmware
// (https://github.com/technomancy/atreus-firmware).

#include "usb_impl.h"

#include "usb_common.h"
#include "usb_descriptors.h"
#include "usb_handlers.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

namespace threeboard {
namespace native {
namespace {

#define EP_SINGLE_BUFFER 0x02

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

ISR(USB_GEN_vect) {
  static uint8_t div4 = 0;
  uint8_t device_interrupt = UDINT;
  UDINT = 0;
  // Detect end of reset interrupt.
  if (device_interrupt & (1 << EORSTI)) {
    // Switch to endpoint 0.
    UENUM = 0;
    // Enable the endpoint.
    // TODO: I don't think this is necessary (datasheet p286).
    UECONX = 1 << EPEN;
    // Set endpoint 0's type as a control endpoint.
    UECFG0X = 0;
    UECFG1X = 0x20 | EP_SINGLE_BUFFER;
    UEIENX = (1 << RXSTPE);
  }
  
  // https://www.keil.com/pack/doc/mw/USB/html/_h_i_d.html
  // idle rate!
  
  if ((device_interrupt & (1 << SOFI)) && hid_state.configuration) {
    if (hid_state.keyboard_idle_config && (++div4 & 3) == 0) {
      UENUM = KEYBOARD_ENDPOINT;
      if (UEINTX & (1 << RWAL)) {
        hid_state.keyboard_idle_count++;
        if (hid_state.keyboard_idle_count == hid_state.keyboard_idle_config) {
          hid_state.keyboard_idle_count = 0;
          UEDATX = hid_state.modifier_keys;
          UEDATX = 0;
          for (uint8_t i = 0; i < 6; i++) {
            UEDATX = hid_state.keyboard_keys[i];
          }
          UEINTX = 0x3A;
        }
      }
    }
  }
}

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
  if (packet.wIndex == KEYBOARD_INTERFACE &&
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
int8_t usb_keyboard_send() {
  uint8_t intr_state = SREG;
  cli();
  UENUM = KEYBOARD_ENDPOINT;
  uint8_t timeout = UDFNUML + 50;
  while (1) {
    // Check if we're allowed to push data into the FIFO. If we are, we can
    // immediately break and begin transmitting.
    if (UEINTX & (1 << RWAL)) {
      break;
    }
    SREG = intr_state;
    // has the USB gone offline?
    if (!hid_state.configuration)
      return -1;
    // Only continue polling RWAL for 50 frames (50ms on our full-speed bus)
    if (UDFNUML >= timeout) {
      return -1;
    }
    // get ready to try checking again
    intr_state = SREG;
    cli();
    UENUM = KEYBOARD_ENDPOINT;
  }

  // Transmit keycode state (modifier keys byte + 6x keyboard key bytes).
  UEDATX = hid_state.modifier_keys;
  UEDATX = 0;
  for (uint8_t i = 0; i < 6; i++) {
    UEDATX = hid_state.keyboard_keys[i];
  }

  UEINTX = 0x3A;
  hid_state.keyboard_idle_count = 0;
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
  // Configure USB interrupts. We want to interrupt on start of frame (SOFE),
  // and also on end of reset (EORSTE).
  UDIEN = (1 << EORSTE) | (1 << SOFE);
  // Enable global interrupts.
  sei();
}

void UsbImpl::SendKeypress(const uint8_t key, const uint8_t mod) {
  hid_state.modifier_keys = mod;
  // Currently we only support sending a single key at a time, even though this
  // USB implementation supports the full 6 keys. Functionality of the
  // threeboard may change in future to send multiple keys.
  hid_state.keyboard_keys[0] = key;
  // TODO: capture errors
  usb_keyboard_send();
  hid_state.modifier_keys = 0;
  hid_state.keyboard_keys[0] = 0;
  // TODO: capture errors
  usb_keyboard_send();
}
} // namespace native
} // namespace threeboard
