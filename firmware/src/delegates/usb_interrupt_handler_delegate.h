#pragma once

namespace threeboard {

class UsbInterruptHandlerDelegate {
public:
  virtual ~UsbInterruptHandlerDelegate() {}
  virtual void HandleGeneralInterrupt() = 0;
  virtual void HandleEndpointInterrupt() = 0;
};
} // namespace threeboard
