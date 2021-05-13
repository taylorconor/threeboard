#pragma once

#include "gmock/gmock.h"
#include "simulator_delegate.h"

namespace threeboard {
namespace simulator {
namespace detail {

class DefaultSimulatorDelegateMock : public SimulatorDelegate {
 public:
  MOCK_METHOD(void, PrepareRenderState, (), (override));
  MOCK_METHOD(void, HandlePhysicalKeypress, (char, bool), (override));
  MOCK_METHOD(void, HandleVirtualKeypress, (uint8_t, uint8_t), (override));
  MOCK_METHOD(void, HandleUartLogLine, (const std::string &), (override));
  MOCK_METHOD(Flags *, GetFlags, (), (override));
  MOCK_METHOD(bool, IsUsbAttached, (), (override));
};
}  // namespace detail

using SimulatorDelegateMock =
    ::testing::StrictMock<detail::DefaultSimulatorDelegateMock>;

}  // namespace simulator
}  // namespace threeboard