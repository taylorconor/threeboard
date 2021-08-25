#pragma once

#include <string>

#include "src/led_state.h"

namespace threeboard {
namespace integration {

struct ObservableState {
  uint8_t bank_0 = 0;
  uint8_t bank_1 = 0;
  LedState::State led_r = LedState::OFF;
  LedState::State led_g = LedState::OFF;
  LedState::State led_b = LedState::OFF;
  LedState::State led_prog = LedState::OFF;
  LedState::State led_err = LedState::OFF;
  LedState::State led_status = LedState::OFF;
  std::string usb_buffer;
};

}  // namespace integration
}  // namespace threeboard