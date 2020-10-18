#include "uart.h"

#include "simulator/simavr/simavr_mock.h"
#include "simulator/simulator_delegate_mock.h"
#include "gtest/gtest.h"

namespace threeboard {
namespace simulator {

using ::testing::_;
using ::testing::Return;

class UartTest : public ::testing::Test {
public:
  UartTest() {
    uint32_t flags = 0;
    // TODO: capture the flags reference here.
    EXPECT_CALL(simavr_mock_, InvokeIoctl(UART_GET_FLAGS, _)).Times(1);
    EXPECT_CALL(simavr_mock_, InvokeIoctl(UART_SET_FLAGS, _)).Times(1);
    // TODO: capture the callback in the argument.
    EXPECT_CALL(simavr_mock_, RegisterUartOutputCallback(_)).Times(1);

    uart_ = std::make_unique<Uart>(&simavr_mock_, &simulator_delegate_mock_);
  }

  SimavrMock simavr_mock_;
  SimulatorDelegateMock simulator_delegate_mock_;
  std::unique_ptr<Uart> uart_;
};

TEST_F(UartTest, FakeTest) {}
} // namespace simulator
} // namespace threeboard