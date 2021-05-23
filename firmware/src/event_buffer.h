#pragma once

#include "src/delegates/event_handler_delegate.h"

namespace threeboard {

// A very basic delegate implementation that allows state to be passed from the
// key controller to the main keypress polling run loop. Keeping this as a
// separate class allows decoupling to avoid a dependency cycle.
class EventBuffer final : public EventHandlerDelegate {
 public:
  ~EventBuffer() override = default;

  bool HasEvent() const;

  // This function is NOT interrupt safe; The caller must take responsibility
  // for this.
  bool HasKeypressEvent() const;
  Keypress GetKeypressEvent();

  // Implement the EventHandlerDelegate override. This method is responsible
  // for handling all keypresses and combos. It runs inside an ISR, so to keep
  // ISR executions as short as possible, it offloads the event to a buffer
  // variable (pending_keypress_) which is picked up in the main program's
  // run loop.
  void HandleKeypress(const Keypress &) override;

 private:
  union Event {
    Keypress keypress_;
  };

  enum class EventType {
    NO_EVENT = 0,
    KEYPRESS = 1,
  };

  Event event_;
  EventType event_type_ = EventType::NO_EVENT;
};
}  // namespace threeboard
