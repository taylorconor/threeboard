// This USB implementation is heavily influenced by the LUFA project
// (https://github.com/abcminiuser/lufa), and by the Atreus firmware
// (https://github.com/technomancy/atreus-firmware).

#include "usb_impl.h"

#include "usb_descriptors.h"
#include "usb_handlers.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

namespace threeboard {
namespace native {
namespace {

#define EP_SINGLE_BUFFER 0x02
// zero when we are not configured, non-zero when enumerated
static volatile uint8_t usb_configuration = 0;

// which modifier keys are currently pressed
// 1=left ctrl,    2=left shift,   4=left alt,    8=left gui
// 16=right ctrl, 32=right shift, 64=right alt, 128=right gui
uint8_t keyboard_modifier_keys = 0;

// which keys are currently pressed, up to 6 keys may be down at once
uint8_t keyboard_keys[6] = {0, 0, 0, 0, 0, 0};

// protocol setting from the host.  We use exactly the same report
// either way, so this variable only stores the setting since we
// are required to be able to report which setting is in use.
static uint8_t keyboard_protocol = 1;

// the idle configuration, how often we send the report to the
// host (ms * 4) even when it hasn't changed
static uint8_t keyboard_idle_config = 125;

// count until idle timeout
static uint8_t keyboard_idle_count = 0;

// 1=num lock, 2=caps lock, 4=scroll lock, 8=compose, 16=kana
volatile uint8_t keyboard_leds = 0;

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
  if ((device_interrupt & (1 << SOFI)) && usb_configuration) {
    if (keyboard_idle_config && (++div4 & 3) == 0) {
      UENUM = KEYBOARD_ENDPOINT;
      if (UEINTX & (1 << RWAL)) {
        keyboard_idle_count++;
        if (keyboard_idle_count == keyboard_idle_config) {
          keyboard_idle_count = 0;
          UEDATX = keyboard_modifier_keys;
          UEDATX = 0;
          for (uint8_t i = 0; i < 6; i++) {
            UEDATX = keyboard_keys[i];
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

  uint8_t i;

  if (packet.bRequest == Request::GET_STATUS) {
    device_handler::HandleGetStatus();
  } else if (packet.bRequest == Request::SET_ADDRESS) {
    device_handler::HandleSetAddress(packet);
  } else if (packet.bRequest == Request::GET_DESCRIPTOR) {
    device_handler::HandleGetDescriptor(packet);
  } else if (packet.bRequest == Request::GET_CONFIGURATION &&
             packet.bmRequestType.GetDirection() ==
                 RequestType::Direction::DEVICE_TO_HOST) {
    device_handler::HandleGetConfiguration(usb_configuration);
  } else if (packet.bRequest == Request::SET_CONFIGURATION &&
             packet.bmRequestType.GetDirection() ==
                 RequestType::Direction::HOST_TO_DEVICE) {
    device_handler::HandleSetConfiguration(packet, &usb_configuration);
  } else if (packet.wIndex == KEYBOARD_INTERFACE &&
             packet.bmRequestType.GetType() == RequestType::Type::CLASS &&
             packet.bmRequestType.GetRecipient() ==
                 RequestType::Recipient::INTERFACE) {
    if (packet.bmRequestType.GetDirection() ==
        RequestType::Direction::DEVICE_TO_HOST) {
      if (packet.bRequest == Request::HID_GET_REPORT) {
        AwaitTransmitterReady();
        UEDATX = keyboard_modifier_keys;
        UEDATX = 0;
        for (i = 0; i < 6; i++) {
          UEDATX = keyboard_keys[i];
        }
        UEDATX = 0;
        return;
      }
      if (packet.bRequest == Request::HID_GET_IDLE) {
        AwaitTransmitterReady();
        UEDATX = keyboard_idle_config;
        HandshakeTransmitterInterrupt();
        return;
      }
      if (packet.bRequest == Request::HID_GET_PROTOCOL) {
        AwaitTransmitterReady();
        UEDATX = keyboard_protocol;
        HandshakeTransmitterInterrupt();
        return;
      }
    }
    if (packet.bmRequestType.GetDirection() ==
        RequestType::Direction::HOST_TO_DEVICE) {
      if (packet.bRequest == Request::HID_SET_REPORT) {
        AwaitReceiverReady();
        keyboard_leds = UEDATX;
        HandshakeReceiverInterrupt();
        HandshakeTransmitterInterrupt();
        return;
      }
      if (packet.bRequest == Request::HID_SET_IDLE) {
        keyboard_idle_config = (packet.wValue >> 8);
        keyboard_idle_count = 0;
        HandshakeTransmitterInterrupt();
        return;
      }
      if (packet.bRequest == Request::HID_SET_PROTOCOL) {
        keyboard_protocol = packet.wValue;
        HandshakeTransmitterInterrupt();
        return;
      }
    }
  }
}

// send the contents of keyboard_keys and keyboard_modifier_keys
int8_t usb_keyboard_send(void) {
  uint8_t i, intr_state, timeout;
  intr_state = SREG;
  cli();
  UENUM = KEYBOARD_ENDPOINT;
  timeout = UDFNUML + 50;
  while (1) {
    // are we ready to transmit?
    if (UEINTX & (1 << RWAL))
      break;
    SREG = intr_state;
    // has the USB gone offline?
    if (!usb_configuration)
      return -1;
    // have we waited too long?
    if (UDFNUML == timeout)
      return -1;
    // get ready to try checking again
    intr_state = SREG;
    cli();
    UENUM = KEYBOARD_ENDPOINT;
  }
  UEDATX = keyboard_modifier_keys;
  UEDATX = 0;
  for (i = 0; i < 6; i++) {
    UEDATX = keyboard_keys[i];
  }
  UEINTX = 0x3A;
  keyboard_idle_count = 0;
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
  keyboard_modifier_keys = mod;
  keyboard_keys[0] = key;
  // TODO: capture errors
  usb_keyboard_send();
  keyboard_modifier_keys = 0;
  keyboard_keys[0] = 0;
  // TODO: capture errors
  usb_keyboard_send();
}
} // namespace native
} // namespace threeboard
