#pragma once

namespace threeboard {
namespace native {

class TimerInterruptHandlerDelegate {
public:
  virtual void HandleTimer1Interrupt() = 0;
  virtual void HandleTimer3Interrupt() = 0;
};
} // namespace native
} // namespace threeboard
