#pragma once

#include "src/delegates/keypress_handler_delegate.h"
#include "src/led_controller.h"
#include "src/native/native.h"

namespace threeboard {

class EventBuffer : public KeypressHandlerDelegate {
public:
  EventBuffer(LedController *led_controller)
      : led_controller_(led_controller) {}

  // Immediately returns the pending keypress event if it's available, otherwise
  // it returns INACTIVE.
  // This function is NOT interrupt safe; The caller must take responsibility
  // for this.
  Keypress GetPendingEventIfAvailable();

  // Implement the KeypressHandlerDelegate override. This method is responsible
  // for handling all keypresses and combos. It runs inside an ISR, so to keep
  // ISR executions as short as possible, it offloads the event to a buffer
  // variable (pending_keypress_) which is picked up in the main program's
  // runloop.
  void HandleKeypress(const Keypress) override;

private:
  Keypress pending_keypress_ = Keypress::INACTIVE;
  LedController *led_controller_;
  bool test_;
};
} // namespace threeboard
