#pragma once

#include "gmock/gmock.h"
#include "led_controller.h"

namespace threeboard {
namespace detail {

class DefaultLedControllerMock : public LedController {
 public:
  MOCK_METHOD(void, ScanNextLine, (), (override));
  MOCK_METHOD(void, UpdateBlinkStatus, (), (override));
  MOCK_METHOD(LedState*, GetLedState, (), (override));
};
}  // namespace detail

using LedControllerMock =
    ::testing::StrictMock<detail::DefaultLedControllerMock>;

}  // namespace threeboard