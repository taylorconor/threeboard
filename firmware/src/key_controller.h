#pragma once

#include "src/delegates/keypress_handler_delegate.h"
#include "src/native/native.h"

namespace threeboard {

// A class to manage keyboard actions and combinations, and offload their
// handling to a provided delegate.
class KeyController {
public:
  KeyController(native::Native *native,
                KeypressHandlerDelegate *keypress_handler);

  // Called by the timer 3 interrupt handler every 5ms.
  void PollKeyState();

private:
  native::Native *native_;
  KeypressHandlerDelegate *keypress_handler_;
  // The current and previous state of the keyboard. Used to store combos until
  // ready to pass to the keypress handler.
  uint8_t key_mask_ = 0;

  bool HasActiveKeypress();
};
} // namespace threeboard
