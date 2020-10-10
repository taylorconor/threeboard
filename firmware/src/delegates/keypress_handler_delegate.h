#pragma once

#include <stdint.h>

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
  virtual void HandleKeypress(const Keypress &) = 0;

protected:
  virtual ~KeypressHandlerDelegate() = default;
};
} // namespace threeboard
