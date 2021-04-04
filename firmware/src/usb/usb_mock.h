#pragma once

#include "gmock/gmock.h"
#include "usb.h"

namespace threeboard {
namespace usb {
namespace detail {

class DefaultUsbMock : public Usb {
 public:
  MOCK_METHOD(bool, Setup, (), (override));
  MOCK_METHOD(bool, HasConfigured, (), (override));
  MOCK_METHOD(bool, SendKeypress, (uint8_t, uint8_t), (override));
};
}  // namespace detail

using UsbMock = ::testing::StrictMock<detail::DefaultUsbMock>;

}  // namespace usb
}  // namespace threeboard