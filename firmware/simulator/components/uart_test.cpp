#include "uart.h"

#include "gtest/gtest.h"
#include "simulator/simavr/simavr_mock.h"
#include "simulator/ui/ui_delegate_mock.h"

namespace threeboard {
namespace simulator {
namespace {

using ::testing::_;
using ::testing::Return;

class UartTest : public ::testing::Test {
 public:
  UartTest() {
    // TODO: capture the flags reference here.
    EXPECT_CALL(simavr_mock_, InvokeIoctl(UART_GET_FLAGS, _)).Times(1);
    EXPECT_CALL(simavr_mock_, InvokeIoctl(UART_SET_FLAGS, _)).Times(1);
    // TODO: capture the callback in the argument.
    EXPECT_CALL(simavr_mock_, RegisterUartOutputCallback(_)).Times(1);

    uart_ = std::make_unique<Uart>(&simavr_mock_, &ui_delegate_mock_);
  }

  SimavrMock simavr_mock_;
  UIDelegateMock ui_delegate_mock_;
  std::unique_ptr<Uart> uart_;
};

// TODO: write tests!
TEST_F(UartTest, FakeTest) {}
}  // namespace
}  // namespace simulator
}  // namespace threeboard