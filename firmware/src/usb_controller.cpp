#include "usb_controller.h"

namespace threeboard {

UsbController::UsbController(native::Native *native) : native_(native) {}

void UsbController::SendKeyToHost(const uint8_t keycode,
                                  const uint8_t modcode) {
  return;
}
} // namespace threeboard
