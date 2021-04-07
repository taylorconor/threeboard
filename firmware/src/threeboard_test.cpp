#include "threeboard.h"

#include <memory>

#include "src/event_buffer.h"
#include "src/key_controller_mock.h"
#include "src/led_controller_mock.h"
#include "src/logging_fake.h"
#include "src/native/native_mock.h"
#include "src/usb/usb_mock.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::Sequence;

namespace threeboard {

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

  void WaitForUsbSetup() { threeboard_->WaitForUsbSetup(); }
  void WaitForUsbConfiguration() { threeboard_->WaitForUsbConfiguration(); }
  void RunEventLoopIteration() { threeboard_->RunEventLoopIteration(); }

  native::NativeMock native_mock_;
  usb::UsbMock usb_mock_;
  EventBuffer event_buffer_;
  LedControllerMock led_controller_mock_;
  KeyControllerMock key_controller_mock_;
  LedState led_state_;
  LoggingFake logging_fake_;
  std::unique_ptr<Threeboard> threeboard_;
};

TEST_F(ThreeboardTest, CorrectlyHandleTimer1Interrupt) {
  EXPECT_CALL(led_controller_mock_, ScanNextLine()).Times(1);
  threeboard_->HandleTimer1Interrupt();
}

TEST_F(ThreeboardTest, CorrectlyHandleTimer3Interrupt) {
  EXPECT_CALL(led_controller_mock_, UpdateBlinkStatus()).Times(1);
  EXPECT_CALL(key_controller_mock_, PollKeyState()).Times(1);
  threeboard_->HandleTimer3Interrupt();
}

TEST_F(ThreeboardTest, RetryOnUsbSetupFailure) {
  // Fail the first two iterations of Setup(), and return true (success) on the
  // third iteration.
  EXPECT_CALL(usb_mock_, Setup())
      .WillOnce(Return(false))
      .WillOnce(Return(false))
      .WillOnce(Return(true));

  EXPECT_CALL(native_mock_, DelayMs(50)).Times(2);

  EXPECT_CALL(led_controller_mock_, GetLedState())
      .Times(3)
      .WillRepeatedly(Return(&led_state_));

  WaitForUsbSetup();
}

TEST_F(ThreeboardTest, RetryOnUsbConfigureFailure) {
  Sequence seq;
  EXPECT_CALL(usb_mock_, HasConfigured())
      .Times(UINT16_MAX)
      .InSequence(seq)
      .WillRepeatedly(Return(false));
  EXPECT_CALL(usb_mock_, HasConfigured())
      .InSequence(seq)
      .WillOnce(Return(true));

  EXPECT_CALL(led_controller_mock_, GetLedState())
      .Times(2)
      .WillRepeatedly(Return(&led_state_));

  WaitForUsbConfiguration();
}

TEST_F(ThreeboardTest, EventLoopIterationWithEvent) {
  // Add an event to the event buffer.
  event_buffer_.HandleKeypress(Keypress::X);
  EXPECT_CALL(native_mock_, DisableInterrupts()).Times(1);
  EXPECT_CALL(native_mock_, EnableInterrupts()).Times(1);

  RunEventLoopIteration();
}

TEST_F(ThreeboardTest, EventLoopIterationWithNoEvent) {
  EXPECT_CALL(native_mock_, DisableInterrupts()).Times(1);
  EXPECT_CALL(native_mock_, EnableCpuSleep()).Times(1);
  EXPECT_CALL(native_mock_, EnableInterrupts()).Times(1);
  EXPECT_CALL(native_mock_, SleepCpu()).Times(1);
  EXPECT_CALL(native_mock_, DisableCpuSleep()).Times(1);

  RunEventLoopIteration();
}
}  // namespace threeboard