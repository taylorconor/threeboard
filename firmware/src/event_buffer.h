#pragma once

#include "src/delegates/keypress_handler_delegate.h"

namespace threeboard {

// A very basic delegate implementation that allows state to be passed from the
// key controller to the main keypress polling run loop. Keeping this as a
// separate class allows decoupling to avoid a dependency cycle.
class EventBuffer : public KeypressHandlerDelegate {
public:
  ~EventBuffer() override = default;

  // Immediately returns the pending keypress event if it's available, otherwise
  // it returns INACTIVE.
  // This function is NOT interrupt safe; The caller must take responsibility
  // for this.
  Keypress GetPendingEventIfAvailable();

  // Implement the KeypressHandlerDelegate override. This method is responsible
  // for handling all keypresses and combos. It runs inside an ISR, so to keep
  // ISR executions as short as possible, it offloads the event to a buffer
  // variable (pending_keypress_) which is picked up in the main program's
  // run loop.
  void HandleKeypress(const Keypress &) override;

private:
  Keypress pending_keypress_ = Keypress::INACTIVE;
};
} // namespace threeboard
