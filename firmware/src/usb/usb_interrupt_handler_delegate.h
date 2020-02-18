#pragma once

namespace threeboard {
namespace usb {

class UsbInterruptHandlerDelegate {
public:
  virtual ~UsbInterruptHandlerDelegate() {}
  virtual void HandleGeneralInterrupt();
  virtual void HandleEndpointInterrupt();
};
} // namespace usb
} // namespace threeboard
