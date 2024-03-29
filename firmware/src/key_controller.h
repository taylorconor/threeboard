#pragma once

#include "src/delegates/event_handler_delegate.h"
#include "src/native/native.h"

namespace threeboard {

// A class to manage keyboard actions and combinations, and offload their
// handling to a provided delegate.
class KeyController {
 public:
  KeyController(native::Native *native, EventHandlerDelegate *keypress_handler);
  virtual ~KeyController() = default;

  // Called by the timer 3 interrupt handler every 5ms.
  virtual void PollKeyState();

 protected:
  // A default constructor used by the KeyControllerMock to avoid the
  // Native-dependent public constructor.
  KeyController() = default;

 private:
  native::Native *native_;
  EventHandlerDelegate *keypress_handler_;

  // The current and previous state of the keyboard. Used to store combos until
  // ready to pass to the keypress handler.
  uint8_t key_mask_;
};
}  // namespace threeboard
