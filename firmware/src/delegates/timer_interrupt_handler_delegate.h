#pragma once

namespace threeboard {

class TimerInterruptHandlerDelegate {
public:
  virtual void HandleTimer1Interrupt() = 0;
  virtual void HandleTimer3Interrupt() = 0;

protected:
  virtual ~TimerInterruptHandlerDelegate() = default;
};
} // namespace threeboard
