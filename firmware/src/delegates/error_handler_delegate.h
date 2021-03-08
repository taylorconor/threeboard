#pragma once

#include <stdint.h>

namespace threeboard {

// An interface that defines all methods needed to be implemented by the
// class(es) that are responsible for handling error events.
class ErrorHandlerDelegate {
 public:
  virtual void HandleUsbSetupError() = 0;

 protected:
  virtual ~ErrorHandlerDelegate() = default;
};
}  // namespace threeboard
