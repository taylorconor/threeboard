#pragma once

#include "src/delegates/event_handler_delegate.h"

namespace threeboard {

// A very basic delegate implementation that allows state to be passed from the
// key controller to the main keypress polling run loop. Keeping this as a
// separate class allows decoupling to avoid a dependency cycle.
class EventBuffer final : public EventHandlerDelegate {
 public:
  ~EventBuffer() override = default;

  // These functions are NOT interrupt safe; The caller must take responsibility
  // for this, e.g. by disabling interrupts before calling.
  bool HasKeypressEvent() const { return has_keypress_; }
  Keypress GetKeypressEvent() {
    has_keypress_ = false;
    return event_;
  };

  // Implement the EventHandlerDelegate override. This method is responsible
  // for handling all keypresses and combos. It runs inside an ISR, so to keep
  // ISR executions as short as possible, it offloads the event to a buffer
  // variable (pending_keypress_) which is picked up in the main program's
  // run loop.
  void HandleKeypress(const Keypress &keypress) override {
    event_ = keypress;
    has_keypress_ = true;
  }

 private:
  Keypress event_;
  bool has_keypress_ = false;
};
}  // namespace threeboard
