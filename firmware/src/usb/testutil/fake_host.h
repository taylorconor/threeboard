#pragma once

#include "src/native/native_mock.h"
#include "src/usb/usb_controller_impl.h"

namespace threeboard {
namespace usb {
namespace testutil {

class FakeHost {
 public:
  FakeHost(native::NativeMock *native_mock,
           UsbControllerImpl *usb_controller_impl);

  void HandleDeviceEnumeration();

 private:
  native::NativeMock *native_mock_;
  UsbControllerImpl *usb_controller_impl_;

  void AddFakeEndpointExpectations(Request request);
};
}  // namespace testutil
}  // namespace usb
}  // namespace threeboard
