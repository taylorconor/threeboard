#pragma once

#include "src/key_controller.h"
#include "src/native/native.h"

namespace threeboard {

class EventHandler : public KeypressHandlerDelegate {
public:
  EventHandler(native::Native *native);

  // Blocking method which returns the latest keyboard event when available, and
  // pops it off the buffer. It will block indefinitely until an event appears.
  Keypress WaitForKeyboardEvent();

  // Implement the KeypressHandlerDelegate override. This method is responsible
  // for handling all keypresses and combos. It runs inside the ISR, so to keep
  // ISR executions as short as possible, it offloads the event to a buffer
  // variable (pending_keypress_) which is picked up in the main program.
  void HandleKeypress(const Keypress) override;

private:
  native::Native *native_;
  Keypress pending_keypress_ = Keypress::INACTIVE;
};
} // namespace threeboard
