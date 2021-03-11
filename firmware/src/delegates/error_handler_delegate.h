#pragma once

#include <stdint.h>

namespace threeboard {

// An interface that defines all methods needed to be implemented by the
// class(es) that are responsible for handling error events.
// TODO: perhaps this is no longer necessary?
class ErrorHandlerDelegate {
 public:
 protected:
  virtual ~ErrorHandlerDelegate() = default;
};
}  // namespace threeboard
