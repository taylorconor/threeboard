#pragma once

#include <utility>

namespace threeboard {
namespace simulator {

std::pair<char, uint8_t> ToUsbKeycodes(char c);
char FromUsbKeycodes(char keycode, uint8_t modcode);

}  // namespace simulator
}  // namespace threeboard