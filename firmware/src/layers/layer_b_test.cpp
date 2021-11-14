#include "src/layers/layer_b.h"

#include "gmock/gmock.h"
#include "src/delegates/layer_controller_delegate_mock.h"
#include "src/logging_fake.h"
#include "src/storage/storage_controller_mock.h"
#include "src/usb/usb_controller_mock.h"

namespace threeboard {
namespace {

using testing::_;
using testing::DoAll;
using testing::Return;
using testing::SetArgPointee;

class LayerBTest : public ::testing::Test {
 public:
  LayerBTest()
      : layer_b_(&led_state_, &usb_controller_mock_, &storage_controller_mock_,
                 &layer_controller_delegate_mock_) {}

  void VerifyLayerLedExpectation(bool prog = false) {
    EXPECT_EQ(led_state_.GetR()->state, LedState::OFF);
    EXPECT_EQ(led_state_.GetG()->state, LedState::OFF);
    EXPECT_EQ(led_state_.GetB()->state, LedState::ON);
    EXPECT_EQ(led_state_.GetProg()->state,
              prog ? LedState::ON : LedState ::OFF);
  }

  void EnterProgMode(int character = 0) {
    EXPECT_TRUE(layer_b_.HandleEvent(Keypress::XY));
    VerifyLayerLedExpectation(true);
  }

  void ExitProgMode() {
    EXPECT_TRUE(layer_b_.HandleEvent(Keypress::XYZ));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetProg()->state, LedState::OFF);
  }

  LoggingFake logging_fake_;
  LedState led_state_;
  usb::UsbControllerMock usb_controller_mock_;
  storage::StorageControllerMock storage_controller_mock_;
  LayerControllerDelegateMock layer_controller_delegate_mock_;
  LayerB layer_b_;
};

TEST_F(LayerBTest, ShortcutIdIncrement) {
  EXPECT_CALL(storage_controller_mock_, GetBlobShortcutLength(1, _))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_b_.HandleEvent(Keypress::X));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetBank0(), 1);
  EXPECT_EQ(led_state_.GetBank1(), 0);
}

TEST_F(LayerBTest, ShortcutIdClear) {
  {
    EXPECT_CALL(storage_controller_mock_, GetBlobShortcutLength(1, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(layer_b_.HandleEvent(Keypress::X));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank0(), 1);
  }
  {
    EXPECT_CALL(storage_controller_mock_, GetBlobShortcutLength(0, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(layer_b_.HandleEvent(Keypress::XZ));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank0(), 0);
  }
}

TEST_F(LayerBTest, ProgKeycodeIncrement) {
  EXPECT_EQ(led_state_.GetBank1(), 0);
  EnterProgMode();
  EXPECT_EQ(led_state_.GetBank1(), 0);
  EXPECT_CALL(storage_controller_mock_, GetBlobShortcutLength(_, _)).Times(0);
  EXPECT_TRUE(layer_b_.HandleEvent(Keypress::X));
  VerifyLayerLedExpectation(true);
  EXPECT_EQ(led_state_.GetBank0(), 1);
  EXPECT_EQ(led_state_.GetBank1(), 0);
}

TEST_F(LayerBTest, ProgKeycodeClear) {
  EnterProgMode();
  {
    EXPECT_TRUE(layer_b_.HandleEvent(Keypress::X));
    VerifyLayerLedExpectation(true);
    EXPECT_EQ(led_state_.GetBank0(), 1);
  }
  {
    EXPECT_TRUE(layer_b_.HandleEvent(Keypress::XZ));
    VerifyLayerLedExpectation(true);
    EXPECT_EQ(led_state_.GetBank0(), 0);
  }
}

TEST_F(LayerBTest, ProgModcodeIncrement) {
  EXPECT_EQ(led_state_.GetBank1(), 0);
  EnterProgMode();
  EXPECT_EQ(led_state_.GetBank1(), 0);
  EXPECT_CALL(storage_controller_mock_, GetBlobShortcutLength(_, _)).Times(0);
  EXPECT_TRUE(layer_b_.HandleEvent(Keypress::Y));
  VerifyLayerLedExpectation(true);
  EXPECT_EQ(led_state_.GetBank0(), 0);
  EXPECT_EQ(led_state_.GetBank1(), 1);
}

TEST_F(LayerBTest, ProgModcodeClear) {
  EnterProgMode();
  {
    EXPECT_TRUE(layer_b_.HandleEvent(Keypress::Y));
    VerifyLayerLedExpectation(true);
    EXPECT_EQ(led_state_.GetBank1(), 1);
  }
  {
    EXPECT_TRUE(layer_b_.HandleEvent(Keypress::YZ));
    VerifyLayerLedExpectation(true);
    EXPECT_EQ(led_state_.GetBank1(), 0);
  }
}

TEST_F(LayerBTest, ProgClearBlobShortcut) {
  EnterProgMode();
  EXPECT_CALL(storage_controller_mock_, ClearBlobShortcut(0))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_b_.HandleEvent(Keypress::XY));
  VerifyLayerLedExpectation(true);
  EXPECT_EQ(led_state_.GetErr()->state, LedState::OFF);
}

TEST_F(LayerBTest, FlushToUsb) {
  EXPECT_CALL(storage_controller_mock_, GetBlobShortcutLength(0, _))
      .WillOnce(Return(true));
  EXPECT_CALL(storage_controller_mock_, SendBlobShortcut(0))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_b_.HandleEvent(Keypress::Z));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetErr()->state, LedState::OFF);
}

TEST_F(LayerBTest, AppendCharacterToBlobShortcut) {
  EnterProgMode();
  EXPECT_CALL(storage_controller_mock_, AppendToBlobShortcut(0, 1, 1))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_b_.HandleEvent(Keypress::X));
  EXPECT_TRUE(layer_b_.HandleEvent(Keypress::Y));
  EXPECT_TRUE(layer_b_.HandleEvent(Keypress::Z));
  VerifyLayerLedExpectation(true);
  EXPECT_EQ(led_state_.GetErr()->state, LedState::OFF);
}

TEST_F(LayerBTest, DisplayShortcutLength) {
  EXPECT_CALL(storage_controller_mock_, GetBlobShortcutLength(0, _))
      .WillOnce(DoAll(SetArgPointee<1>(123), Return(true)));
  EXPECT_TRUE(layer_b_.HandleEvent(Keypress::Y));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetBank0(), 0);
  EXPECT_EQ(led_state_.GetBank1(), 123);
}

TEST_F(LayerBTest, ExitProgMode) {
  EnterProgMode();
  EXPECT_CALL(storage_controller_mock_, GetBlobShortcutLength(0, _))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_b_.HandleEvent(Keypress::XYZ));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetProg()->state, LedState::OFF);
}

TEST_F(LayerBTest, LayerSwitch) {
  EXPECT_CALL(layer_controller_delegate_mock_, SwitchToLayer(LayerId::DFLT))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_b_.HandleEvent(Keypress::XYZ));
}

TEST_F(LayerBTest, LayerSwitchFailure) {
  EXPECT_CALL(layer_controller_delegate_mock_, SwitchToLayer(LayerId::DFLT))
      .WillOnce(Return(false));
  EXPECT_FALSE(layer_b_.HandleEvent(Keypress::XYZ));
}

TEST_F(LayerBTest, TransitionToLayer) {
  EXPECT_CALL(storage_controller_mock_, GetBlobShortcutLength(0, _))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_b_.TransitionedToLayer());
  VerifyLayerLedExpectation();
}

TEST_F(LayerBTest, TransitionToFailure) {
  EXPECT_CALL(storage_controller_mock_, GetBlobShortcutLength(0, _))
      .WillOnce(Return(false));
  EXPECT_FALSE(layer_b_.TransitionedToLayer());
  EXPECT_EQ(led_state_.GetB()->state, LedState::OFF);
}

}  // namespace
}  // namespace threeboard