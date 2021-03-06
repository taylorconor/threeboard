#include "src/key_controller.h"

#include <memory>

#include "src/delegates/event_handler_delegate_mock.h"
#include "src/native/native_mock.h"

namespace threeboard {
namespace {

using testing::_;
using testing::Return;

class KeyControllerTest : public ::testing::Test {
 public:
  KeyControllerTest() {
    EXPECT_CALL(native_mock_, DisableDDRB(0b00001110)).Times(1);
    EXPECT_CALL(native_mock_, EnablePORTB(0b00001110)).Times(1);
    controller_ =
        std::make_unique<KeyController>(&native_mock_, &delegate_mock_);
  }

  native::NativeMock native_mock_;
  EventHandlerDelegateMock delegate_mock_;
  std::unique_ptr<KeyController> controller_;
};

TEST_F(KeyControllerTest, DontHandleKeypressWhenNoKeysPressed) {
  EXPECT_CALL(native_mock_, GetPINB()).Times(1).WillOnce(Return(0xFF));
  EXPECT_CALL(delegate_mock_, HandleKeypress(_)).Times(0);
  controller_->PollKeyState();
}

TEST_F(KeyControllerTest, HandleKeypressImmediatelyAfterKeyup) {
  // Press key X for the duration of 10 keystate polls.
  EXPECT_CALL(native_mock_, GetPINB())
      .Times(10)
      .WillRepeatedly(Return(~(1 << native::PB2)));
  EXPECT_CALL(delegate_mock_, HandleKeypress(_)).Times(0);
  for (int i = 0; i < 10; i++) {
    controller_->PollKeyState();
  }
  // Release key X and poll keystate.
  EXPECT_CALL(native_mock_, GetPINB()).Times(2).WillRepeatedly(Return(0xFF));
  EXPECT_CALL(delegate_mock_, HandleKeypress(Keypress::X)).Times(1);
  controller_->PollKeyState();
  // Poll again. Remaining polls should have no effect.
  controller_->PollKeyState();
}

TEST_F(KeyControllerTest, HandleSubKeypressAsComboAtTotalKeyup) {
  // Press key X for the duration of 10 keystate polls.
  EXPECT_CALL(delegate_mock_, HandleKeypress(_)).Times(0);
  EXPECT_CALL(native_mock_, GetPINB())
      .Times(10)
      .WillRepeatedly(Return(~(1 << native::PB2)));
  for (int i = 0; i < 10; i++) {
    controller_->PollKeyState();
  }
  // Press keys X and Y for a single keystate poll.
  EXPECT_CALL(native_mock_, GetPINB())
      .Times(1)
      .WillOnce(Return(~((1 << native::PB2) | (1 << native::PB3))));
  controller_->PollKeyState();
  // Press key X for another 10 keystate polls.
  EXPECT_CALL(native_mock_, GetPINB())
      .Times(10)
      .WillRepeatedly(Return(~(1 << native::PB2)));
  EXPECT_CALL(delegate_mock_, HandleKeypress(_)).Times(0);
  for (int i = 0; i < 10; i++) {
    controller_->PollKeyState();
  }
  // Keyup now should register keycombo XY.
  EXPECT_CALL(native_mock_, GetPINB()).Times(1).WillOnce(Return(0xFF));
  EXPECT_CALL(delegate_mock_, HandleKeypress(Keypress::XY)).Times(1);
  controller_->PollKeyState();
}

TEST_F(KeyControllerTest, HandleOverlappingKeypressesAsComboAtTotalKeyup) {
  // Press key X for the duration of 10 keystate polls.
  EXPECT_CALL(delegate_mock_, HandleKeypress(_)).Times(0);
  EXPECT_CALL(native_mock_, GetPINB())
      .Times(10)
      .WillRepeatedly(Return(~(1 << native::PB2)));
  for (int i = 0; i < 10; i++) {
    controller_->PollKeyState();
  }
  // Press keys XY for the duration of 10 keystate polls.
  EXPECT_CALL(native_mock_, GetPINB())
      .Times(10)
      .WillRepeatedly(Return(~((1 << native::PB2) | (1 << native::PB3))));
  for (int i = 0; i < 10; i++) {
    controller_->PollKeyState();
  }
  // Press key Y for the duration of 10 keystate polls.
  EXPECT_CALL(native_mock_, GetPINB())
      .Times(10)
      .WillRepeatedly(Return(~(1 << native::PB3)));
  for (int i = 0; i < 10; i++) {
    controller_->PollKeyState();
  }
  // Release both keys, poll keystate, and check that the combo was registered.
  EXPECT_CALL(native_mock_, GetPINB()).Times(1).WillOnce(Return(0xFF));
  EXPECT_CALL(delegate_mock_, HandleKeypress(Keypress::XY)).Times(1);
  controller_->PollKeyState();
}

TEST_F(KeyControllerTest, HandleImmediatelyConsecutiveKeypressesAsCombo) {
  // Press key X for the duration of 10 keystate polls.
  EXPECT_CALL(delegate_mock_, HandleKeypress(_)).Times(0);
  EXPECT_CALL(native_mock_, GetPINB())
      .Times(10)
      .WillRepeatedly(Return(~(1 << native::PB2)));
  for (int i = 0; i < 10; i++) {
    controller_->PollKeyState();
  }
  // Release key X and press key Y for the duration of 10 keystate polls.
  EXPECT_CALL(native_mock_, GetPINB())
      .Times(10)
      .WillRepeatedly(Return(~(1 << native::PB3)));
  for (int i = 0; i < 10; i++) {
    controller_->PollKeyState();
  }
  // Release key Y.
  EXPECT_CALL(native_mock_, GetPINB()).Times(1).WillOnce(Return(0xFF));
  EXPECT_CALL(delegate_mock_, HandleKeypress(Keypress::XY)).Times(1);
  controller_->PollKeyState();
}

TEST_F(KeyControllerTest, HandleConsecutiveKeypressesAsIndependent) {
  // Press key X for the duration of 10 keystate polls.
  EXPECT_CALL(delegate_mock_, HandleKeypress(_)).Times(0);
  EXPECT_CALL(native_mock_, GetPINB())
      .Times(10)
      .WillRepeatedly(Return(~(1 << native::PB2)));
  for (int i = 0; i < 10; i++) {
    controller_->PollKeyState();
  }

  // Release key X.
  EXPECT_CALL(delegate_mock_, HandleKeypress(Keypress::X)).Times(1);
  EXPECT_CALL(native_mock_, GetPINB()).Times(1).WillOnce(Return(0xFF));
  controller_->PollKeyState();

  // Press key Y for the duration of 10 keystate polls.
  EXPECT_CALL(native_mock_, GetPINB())
      .Times(10)
      .WillRepeatedly(Return(~(1 << native::PB3)));
  for (int i = 0; i < 10; i++) {
    controller_->PollKeyState();
  }
  // Release key Y.
  EXPECT_CALL(native_mock_, GetPINB()).Times(1).WillOnce(Return(0xFF));
  EXPECT_CALL(delegate_mock_, HandleKeypress(Keypress::Y)).Times(1);
  controller_->PollKeyState();
}
}  // namespace
}  // namespace threeboard
