#pragma once

#include "src/delegates/keypress_handler_delegate.h"
#include "src/native/native.h"

namespace threeboard {

class EventBuffer : public KeypressHandlerDelegate {
public:
  // Immediately returns the pending keypress event if it's available, otherwise
  // it returns INACTIVE.
  Keypress GetPendingEventIfAvailable();

  // Implement the KeypressHandlerDelegate override. This method is responsible
  // for handling all keypresses and combos. It runs inside the ISR, so to keep
  // ISR executions as short as possible, it offloads the event to a buffer
  // variable (pending_keypress_) which is picked up in the main program's
  // runloop.
  void HandleKeypress(const Keypress) override;

private:
  Keypress pending_keypress_ = Keypress::INACTIVE;
};
} // namespace threeboard
