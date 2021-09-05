#pragma once

#include "gmock/gmock.h"
#include "ui_delegate.h"

namespace threeboard {
namespace simulator {
namespace detail {

class DefaultUIDelegateMock : public UIDelegate {
 public:
  MOCK_METHOD(void, HandleLogLine, (const std::string&), (override));
  MOCK_METHOD(void, HandleLogLine, (const std::string&, const SimulatorSource&),
              (override));
};
}  // namespace detail

using UIDelegateMock = ::testing::StrictMock<detail::DefaultUIDelegateMock>;

}  // namespace simulator
}  // namespace threeboard