#pragma once

#include "native/native.h"

namespace threeboard {
class UsbController {
public:
  UsbController(native::Native *native);

  void SendKeyToHost(const uint8_t keycode, const uint8_t modcode);

private:
  native::Native *native_;
};
} // namespace threeboard
