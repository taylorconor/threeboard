#pragma once

#include "src/native/native_mock.h"
#include "src/usb/internal/protocol.h"
#include "src/usb/usb_impl.h"

namespace threeboard {
namespace usb {
namespace testutil {

class FakeHost {
public:
  FakeHost(native::NativeMock *native_mock, UsbImpl *usb_impl);

  void HandleDeviceEnumeration();

private:
  native::NativeMock *native_mock_;
  UsbImpl *usb_impl_;

  void SendBytes(const std::vector<uint8_t> &data);
  std::vector<uint8_t> ReceiveBytes(int size);

  void AddFakeEndpointExpectations(Request request);
};
} // namespace testutil
} // namespace usb
} // namespace threeboard
