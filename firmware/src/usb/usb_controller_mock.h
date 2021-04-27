#pragma once

#include "gmock/gmock.h"
#include "usb_controller.h"

namespace threeboard {
namespace usb {
namespace detail {

class DefaultUsbControllerMock : public UsbController {
 public:
  MOCK_METHOD(bool, Setup, (), (override));
  MOCK_METHOD(bool, HasConfigured, (), (override));
  MOCK_METHOD(bool, SendKeypress, (uint8_t, uint8_t), (override));
};
}  // namespace detail

using UsbControllerMock =
    ::testing::StrictMock<detail::DefaultUsbControllerMock>;

}  // namespace usb
}  // namespace threeboard