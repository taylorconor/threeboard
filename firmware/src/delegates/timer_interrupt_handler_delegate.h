#pragma once

namespace threeboard {

class TimerInterruptHandlerDelegate {
public:
  virtual ~TimerInterruptHandlerDelegate() {}
  virtual void HandleTimer1Interrupt() = 0;
  virtual void HandleTimer3Interrupt() = 0;
};
} // namespace threeboard
