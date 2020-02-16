#pragma once

#include "src/native/native.h"

namespace threeboard {
enum class Keypress : uint8_t {
  // Define an inactive keypress (no keys pressed) to differentiate between
  // active and inactive states in handler delegates. The KeypressHandler will
  // never be called with an INACTIVE keypress.
  INACTIVE = 0,
  // Individual kepresses.
  Z = 1,
  Y = 2,
  X = 4,
  // Two-key combos.
  YZ = 3,
  XZ = 5,
  XY = 6,
  // Three-key combo.
  XYZ = 7,
};

// An interface that defines the delegation of keypress handling, which is used
// by the KeyController.
class KeypressHandlerDelegate {
public:
  virtual ~KeypressHandlerDelegate(){};
  virtual void HandleKeypress(const Keypress) = 0;
};

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
