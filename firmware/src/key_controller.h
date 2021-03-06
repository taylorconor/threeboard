#pragma once

#include "src/delegates/event_handler_delegate.h"
#include "src/native/native.h"

namespace threeboard {

// A class to manage keyboard actions and combinations, and offload their
// handling to a provided delegate.
//
// TODO: This is a very basic implementation of a keyboard controller. It lacks:
// - Debouncing (it assumes Cherry MX switches with 5ms debounce profile).
// - Key repetition.
// - Event handling outside the ISR.
// - More intelligent key combo registration.
class KeyController {
 public:
  KeyController(native::Native *native, EventHandlerDelegate *keypress_handler);

  // Called by the timer 3 interrupt handler every 5ms.
  void PollKeyState();

 private:
  native::Native *native_;
  EventHandlerDelegate *keypress_handler_;

  // The current and previous state of the keyboard. Used to store combos until
  // ready to pass to the keypress handler.
  uint8_t key_mask_;
};
}  // namespace threeboard
