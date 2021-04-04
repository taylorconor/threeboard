#pragma once

#include "gmock/gmock.h"
#include "key_controller.h"

namespace threeboard {
namespace detail {

class DefaultKeyControllerMock : public KeyController {
 public:
  MOCK_METHOD(void, PollKeyState, (), (override));
};
}  // namespace detail

using KeyControllerMock =
    ::testing::StrictMock<detail::DefaultKeyControllerMock>;

}  // namespace threeboard