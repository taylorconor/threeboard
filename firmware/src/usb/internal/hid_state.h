#pragma once

#include <stdint.h>

namespace threeboard {
namespace usb {

// State of the HID device which can be read and mutated by the USB handlers.
struct HidState {
  // The configuration of the USB device, set by the host. Zero when not
  // configured, non-zero after enumeration.
  volatile uint8_t configuration = 0;

  // Currently pressed modifier keys. Zero when not pressed.
  uint8_t modifier_keys = 0;

  // Currently pressed keyboard keys. USB supports up to 6 concurrent
  // keypresses, plus modifier key byte. Zero when not pressed.
  uint8_t keyboard_keys[6] = {0, 0, 0, 0, 0, 0};

  // Protocol setting from the host. This is unused, but we need to store it so
  // we can report it back to the host in GetProtocol.
  uint8_t protocol = 1;

  // Host-configurable reporting timeout when idle, in ms. We send a new HID
  // report to the host every `idle_config` ms, even if nothing has changed.
  uint8_t idle_config = 125;

  // Used to count up to `idle_config` for HID reporting.
  uint8_t idle_count = 0;
};
}  // namespace usb
}  // namespace threeboard
