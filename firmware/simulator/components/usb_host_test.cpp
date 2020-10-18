#include "usb_host.h"

#include "simulator/simavr/simavr_mock.h"
#include "simulator/simulator_delegate_mock.h"
#include "gtest/gtest.h"

namespace threeboard {
namespace simulator {

using ::testing::_;
using ::testing::Return;

class UsbHostTest : public ::testing::Test {
public:
  UsbHostTest() {
    // TODO: capture callback here.
    EXPECT_CALL(simavr_mock_, RegisterUsbAttachCallback(_)).Times(1);

    usb_host_ =
        std::make_unique<UsbHost>(&simavr_mock_, &simulator_delegate_mock_);
  }

  SimavrMock simavr_mock_;
  SimulatorDelegateMock simulator_delegate_mock_;
  std::unique_ptr<UsbHost> usb_host_;
};

TEST_F(UsbHostTest, FakeTest) {}
} // namespace simulator
} // namespace threeboard