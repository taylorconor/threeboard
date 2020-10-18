#pragma once

#include "simulator_delegate.h"

#include "gmock/gmock.h"

namespace threeboard {
namespace simulator {
namespace detail {

class DefaultSimulatorDelegateMock : public SimulatorDelegate {
public:
  MOCK_METHOD(void, PrepareRenderState, (), (override));
  MOCK_METHOD(void, HandlePhysicalKeypress, (char, bool), (override));
  MOCK_METHOD(void, HandleVirtualKeypress, (uint8_t, uint8_t), (override));
  MOCK_METHOD(void, HandleUartLogLine, (const std::string &), (override));
  MOCK_METHOD(uint16_t, GetGdbPort, (), (override));
  MOCK_METHOD(bool, IsUsbAttached, (), (override));
};
} // namespace detail

using SimulatorDelegateMock =
    ::testing::StrictMock<detail::DefaultSimulatorDelegateMock>;

} // namespace simulator
} // namespace threeboard