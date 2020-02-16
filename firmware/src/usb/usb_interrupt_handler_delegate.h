#pragma once

namespace threeboard {
namespace usb {

class UsbInterruptHandlerDelegate {
public:
  virtual void HandleGeneralInterrupt();
  virtual void HandleEndpointInterrupt();
};
} // namespace usb
} // namespace threeboard
