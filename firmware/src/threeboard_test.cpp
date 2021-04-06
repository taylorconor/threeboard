#include "threeboard.h"

#include <memory>

#include "src/event_buffer.h"
#include "src/key_controller_mock.h"
#include "src/led_controller_mock.h"
#include "src/native/native_mock.h"
#include "src/usb/usb_mock.h"

using ::testing::_;
using ::testing::Return;

namespace threeboard {
namespace {

class ThreeboardTest : public ::testing::Test {
 public:
  ThreeboardTest() {
    EXPECT_CALL(native_mock_, SetTimerInterruptHandlerDelegate(_)).Times(1);
    EXPECT_CALL(native_mock_, EnableTimer1()).Times(1);
    EXPECT_CALL(native_mock_, EnableTimer3()).Times(1);
    EXPECT_CALL(led_controller_mock_, GetLedState())
        .WillOnce(Return(&led_state_));
    threeboard_ = std::make_unique<Threeboard>(
        &native_mock_, &usb_mock_, &event_buffer_, &led_controller_mock_,
        &key_controller_mock_);
  }

  native::NativeMock native_mock_;
  usb::UsbMock usb_mock_;
  EventBuffer event_buffer_;
  LedControllerMock led_controller_mock_;
  KeyControllerMock key_controller_mock_;
  LedState led_state_;
  std::unique_ptr<Threeboard> threeboard_;
};

TEST_F(ThreeboardTest, FakeTest) {}
}  // namespace
}  // namespace threeboard