#pragma once

#include "src/keypress.h"

namespace threeboard {

// An interface that defines the methods needed to be implemented for event
// handling.
class EventHandlerDelegate {
 public:
  virtual void HandleKeypress(const Keypress &) = 0;

 protected:
  virtual ~EventHandlerDelegate() = default;
};
}  // namespace threeboard
