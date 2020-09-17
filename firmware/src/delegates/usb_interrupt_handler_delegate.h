#pragma once

namespace threeboard {

class UsbInterruptHandlerDelegate {
public:
  virtual void HandleGeneralInterrupt() = 0;
  virtual void HandleEndpointInterrupt() = 0;

protected:
  virtual ~UsbInterruptHandlerDelegate() = default;
};
} // namespace threeboard
