#pragma once

#include <stdint.h>

#include <utility>

namespace threeboard {
namespace simulator {

// Converts a character to a pair of (keycode, modcode).
std::pair<char, uint8_t> ToUsbKeycodes(char c);

// Converts a (keycode, modcode) pair to an ascii character.
char FromUsbKeycodes(char keycode, uint8_t modcode);

}  // namespace simulator
}  // namespace threeboard
