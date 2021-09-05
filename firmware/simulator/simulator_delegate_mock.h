#pragma once

#include "gmock/gmock.h"
#include "simulator_delegate.h"

namespace threeboard {
namespace simulator {
namespace detail {

class DefaultSimulatorDelegateMock : public SimulatorDelegate {
 public:
  MOCK_METHOD(void, HandleUsbOutput, (uint8_t, uint8_t), (override));
};
}  // namespace detail

using SimulatorDelegateMock =
    ::testing::StrictMock<detail::DefaultSimulatorDelegateMock>;

}  // namespace simulator
}  // namespace threeboard