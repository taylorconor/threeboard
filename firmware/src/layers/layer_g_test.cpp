#include "src/layers/layer_g.h"

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

class LayerGTest : public ::testing::Test {
 public:
  LayerGTest()
      : layer_g_(&led_state_, &usb_controller_mock_, &storage_controller_mock_,
                 &layer_controller_delegate_mock_) {}

  void VerifyLayerLedExpectation(bool prog = false) {
    EXPECT_EQ(led_state_.GetR()->state, LedState::OFF);
    EXPECT_EQ(led_state_.GetG()->state, LedState::ON);
    EXPECT_EQ(led_state_.GetB()->state, LedState::OFF);
    EXPECT_EQ(led_state_.GetProg()->state,
              prog ? LedState::ON : LedState ::OFF);
  }

  void EnterProgMode(int length = 0) {
    EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(_, _))
        .WillOnce(DoAll(SetArgPointee<1>(length), Return(true)));
    EXPECT_TRUE(layer_g_.HandleEvent(Keypress::XY));
    VerifyLayerLedExpectation(true);
  }

  void ExitProgMode() {
    EXPECT_TRUE(layer_g_.HandleEvent(Keypress::XYZ));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetProg()->state, LedState::OFF);
  }

  LoggingFake logging_fake_;
  LedState led_state_;
  usb::UsbControllerMock usb_controller_mock_;
  storage::StorageControllerMock storage_controller_mock_;
  LayerControllerDelegateMock layer_controller_delegate_mock_;
  LayerG layer_g_;
};

TEST_F(LayerGTest, ShortcutIdIncrement) {
  EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(1, _))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_g_.HandleEvent(Keypress::X));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetBank0(), 1);
  EXPECT_EQ(led_state_.GetBank1(), 0);
}

TEST_F(LayerGTest, ShortcutIdClear) {
  {
    EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(1, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(layer_g_.HandleEvent(Keypress::X));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank0(), 1);
  }
  {
    EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(layer_g_.HandleEvent(Keypress::XZ));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank0(), 0);
  }
}

TEST_F(LayerGTest, ProgKeycodeIncrement) {
  EXPECT_EQ(led_state_.GetBank1(), 0);
  EnterProgMode();
  EXPECT_EQ(led_state_.GetBank1(), 0);
  EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_g_.HandleEvent(Keypress::X));
  VerifyLayerLedExpectation(true);
  EXPECT_EQ(led_state_.GetBank0(), 1);
}

TEST_F(LayerGTest, ProgKeycodeClear) {
  EnterProgMode();
  {
    EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(layer_g_.HandleEvent(Keypress::X));
    VerifyLayerLedExpectation(true);
    EXPECT_EQ(led_state_.GetBank0(), 1);
  }
  {
    EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(layer_g_.HandleEvent(Keypress::XZ));
    VerifyLayerLedExpectation(true);
    EXPECT_EQ(led_state_.GetBank0(), 0);
  }
}

TEST_F(LayerGTest, WordModCodeIncrement) {
  EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_g_.HandleEvent(Keypress::Y));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetBank0(), 0);
  EXPECT_EQ(led_state_.GetBank1(), 1);
}

TEST_F(LayerGTest, WordModCodeClear) {
  {
    EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(layer_g_.HandleEvent(Keypress::Y));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank1(), 1);
  }
  {
    EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(layer_g_.HandleEvent(Keypress::YZ));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank1(), 0);
  }
}

TEST_F(LayerGTest, ProgClearWordShortcut) {
  EnterProgMode();
  EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
      .WillOnce(Return(true));
  EXPECT_CALL(storage_controller_mock_, ClearWordShortcut(0))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_g_.HandleEvent(Keypress::YZ));
  VerifyLayerLedExpectation(true);
  EXPECT_EQ(led_state_.GetErr()->state, LedState::OFF);
}

TEST_F(LayerGTest, FlushToUsb) {
  EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
      .WillOnce(Return(true));
  EXPECT_CALL(storage_controller_mock_, SendWordShortcut(0, 0))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_g_.HandleEvent(Keypress::Z));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetErr()->state, LedState::OFF);
}

TEST_F(LayerGTest, AppendCharacterToWordShortcut) {
  EnterProgMode();
  EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
      .WillOnce(Return(true));
  EXPECT_CALL(storage_controller_mock_, AppendToWordShortcut(0, 0))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_g_.HandleEvent(Keypress::Z));
  VerifyLayerLedExpectation(true);
  EXPECT_EQ(led_state_.GetErr()->state, LedState::OFF);
}

TEST_F(LayerGTest, DisplayShortcutLength) {
  EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
      .WillOnce(DoAll(SetArgPointee<1>(7), Return(true)));
  EXPECT_TRUE(layer_g_.HandleEvent(Keypress::Y));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetBank0(), 0);
  EXPECT_EQ(led_state_.GetBank1(), (7 << 4) | 1);
}

TEST_F(LayerGTest, EnterProgMode) {
  EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_g_.HandleEvent(Keypress::XY));
  VerifyLayerLedExpectation(true);
}

TEST_F(LayerGTest, ExitProgMode) {
  EnterProgMode();
  EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_g_.HandleEvent(Keypress::XYZ));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetProg()->state, LedState::OFF);
}

TEST_F(LayerGTest, LayerSwitch) {
  EXPECT_CALL(layer_controller_delegate_mock_, SwitchToLayer(LayerId::B))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_g_.HandleEvent(Keypress::XYZ));
}

TEST_F(LayerGTest, LayerSwitchFailure) {
  EXPECT_CALL(layer_controller_delegate_mock_, SwitchToLayer(LayerId::B))
      .WillOnce(Return(false));
  EXPECT_FALSE(layer_g_.HandleEvent(Keypress::XYZ));
}

TEST_F(LayerGTest, TransitionToLayer) {
  EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_g_.TransitionedToLayer());
  VerifyLayerLedExpectation();
}

TEST_F(LayerGTest, TransitionToFailure) {
  EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(0, _))
      .WillOnce(Return(false));
  EXPECT_FALSE(layer_g_.TransitionedToLayer());
  EXPECT_EQ(led_state_.GetG()->state, LedState::OFF);
}

}  // namespace
}  // namespace threeboard