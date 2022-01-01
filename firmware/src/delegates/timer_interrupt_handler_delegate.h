#pragma once

namespace threeboard {

// An interface that enables the Native code to propagate timer interrupts to a
// delegate.
class TimerInterruptHandlerDelegate {
 public:
  virtual void HandleTimer1Interrupt() = 0;
  virtual void HandleTimer3Interrupt() = 0;

 protected:
  virtual ~TimerInterruptHandlerDelegate() = default;
};
}  // namespace threeboard
