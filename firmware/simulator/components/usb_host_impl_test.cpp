#include "usb_host_impl.h"

#include "gtest/gtest.h"
#include "simulator/simavr/simavr_mock.h"
#include "simulator/simulator_delegate_mock.h"

namespace threeboard {
namespace simulator {
namespace {

using ::testing::_;
using ::testing::Return;

class UsbHostTest : public ::testing::Test {
 public:
  UsbHostTest() {
    // TODO: capture callback here.
    EXPECT_CALL(simavr_mock_, RegisterUsbAttachCallback(_)).Times(1);

    usb_host_ =
        std::make_unique<UsbHostImpl>(&simavr_mock_, &simulator_delegate_mock_);
  }

  SimavrMock simavr_mock_;
  SimulatorDelegateMock simulator_delegate_mock_;
  std::unique_ptr<UsbHost> usb_host_;
};

// TODO: write tests!
TEST_F(UsbHostTest, FakeTest) {}
}  // namespace
}  // namespace simulator
}  // namespace threeboard