#pragma once

#include <stdint.h>

namespace threeboard {
namespace native {
struct UsbHidState {
  // The configuration of the USB device, set by the host. Zero when not
  // configured, non-zero after enumeration.
  volatile uint8_t configuration = 0;

  // Currently pressed modifier keys. Zero when not pressed.
  uint8_t modifier_keys = 0;

  // Currently pressed keyboard keys. USB supports up to 6 concurrent
  // keypresses, plus modifier key byte. Zero when not pressed.
  uint8_t keyboard_keys[6] = {0, 0, 0, 0, 0, 0};

  // protocol setting from the host.  We use exactly the same report
  // either way, so this variable only stores the setting since we
  // are required to be able to report which setting is in use.
  uint8_t keyboard_protocol = 1;

  // the idle configuration, how often we send the report to the
  // host (ms * 4) even when it hasn't changed
  uint8_t keyboard_idle_config = 125;

  // count until idle timeout
  uint8_t keyboard_idle_count = 0;
};
} // namespace native
} // namespace threeboard
