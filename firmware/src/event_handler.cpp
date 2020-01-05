#include "event_handler.h"

namespace threeboard {

EventHandler::EventHandler(native::Native *native) : native_(native) {}

Keypress EventHandler::WaitForKeyboardEvent() {
  while (pending_keypress_ == Keypress::INACTIVE) {
    native_->Delay(1);
  }
  auto keypress = pending_keypress_;
  pending_keypress_ = Keypress::INACTIVE;
  return keypress;
}

void EventHandler::HandleKeypress(const Keypress keypress) {
  pending_keypress_ = keypress;
}

} // namespace threeboard
