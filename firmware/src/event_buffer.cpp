#include "src/event_buffer.h"

namespace threeboard {

bool EventBuffer::HasEvent() const {
  return event_type_ != EventType::NO_EVENT;
}

bool EventBuffer::HasKeypressEvent() const {
  return event_type_ == EventType::KEYPRESS;
}

Keypress EventBuffer::GetKeypressEvent() {
  event_type_ = EventType::NO_EVENT;
  return event_.keypress_;
}

void EventBuffer::HandleKeypress(const Keypress &keypress) {
  event_.keypress_ = keypress;
  event_type_ = EventType::KEYPRESS;
}

}  // namespace threeboard
