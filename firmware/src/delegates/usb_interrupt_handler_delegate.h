#pragma once

namespace threeboard {

// An interface that allows the Native code to propagate usb interrupts to a
// delegate.
class UsbInterruptHandlerDelegate {
 public:
  virtual void HandleGeneralInterrupt() = 0;
  virtual void HandleEndpointInterrupt() = 0;

 protected:
  virtual ~UsbInterruptHandlerDelegate() = default;
};
}  // namespace threeboard
