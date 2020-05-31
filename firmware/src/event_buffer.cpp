#include "event_buffer.h"

namespace threeboard {

Keypress EventBuffer::GetPendingEventIfAvailable() {
  auto keypress = pending_keypress_;
  pending_keypress_ = Keypress::INACTIVE;
  return keypress;
}

void EventBuffer::HandleKeypress(const Keypress keypress) {
  pending_keypress_ = keypress;
}

} // namespace threeboard
