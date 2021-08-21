#include "src/layers/layer_r.h"

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

class LayerRTest : public ::testing::Test {
 public:
  LayerRTest()
      : layer_r_(&led_state_, &usb_controller_mock_, &storage_controller_mock_,
                 &layer_controller_delegate_mock_) {}

  void VerifyLayerLedExpectation(bool prog = false) {
    EXPECT_EQ(led_state_.GetR()->state, LedState::ON);
    EXPECT_EQ(led_state_.GetG()->state, LedState::OFF);
    EXPECT_EQ(led_state_.GetB()->state, LedState::OFF);
    EXPECT_EQ(led_state_.GetProg()->state,
              prog ? LedState::ON : LedState ::OFF);
  }

  void EnterProgMode(int character = 0) {
    EXPECT_CALL(storage_controller_mock_, GetCharacterShortcut(_, _))
        .WillOnce(DoAll(SetArgPointee<1>(character), Return(true)));
    EXPECT_TRUE(layer_r_.HandleEvent(Keypress::XY));
    VerifyLayerLedExpectation(true);
  }

  void ExitProgMode() {
    EXPECT_TRUE(layer_r_.HandleEvent(Keypress::XYZ));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetProg()->state, LedState::OFF);
  }

  LoggingFake logging_fake_;
  LedState led_state_;
  usb::UsbControllerMock usb_controller_mock_;
  storage::StorageControllerMock storage_controller_mock_;
  LayerControllerDelegateMock layer_controller_delegate_mock_;
  LayerR layer_r_;
};

TEST_F(LayerRTest, ShortcutIdIncrement) {
  EXPECT_TRUE(layer_r_.HandleEvent(Keypress::X));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetBank0(), 1);
  EXPECT_EQ(led_state_.GetBank1(), 0);
}

TEST_F(LayerRTest, ProgShortcutIdIncrement) {
  EXPECT_EQ(led_state_.GetBank1(), 0);
  EnterProgMode();
  EXPECT_CALL(storage_controller_mock_, GetCharacterShortcut(1, _))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_r_.HandleEvent(Keypress::Y));
  VerifyLayerLedExpectation(true);
  EXPECT_EQ(led_state_.GetBank1(), 1);
}

TEST_F(LayerRTest, ProgShortcutIdClear) {
  EnterProgMode();
  {
    EXPECT_CALL(storage_controller_mock_, GetCharacterShortcut(1, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(layer_r_.HandleEvent(Keypress::Y));
    VerifyLayerLedExpectation(true);
    EXPECT_EQ(led_state_.GetBank1(), 1);
  }
  {
    EXPECT_CALL(storage_controller_mock_, GetCharacterShortcut(0, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(layer_r_.HandleEvent(Keypress::YZ));
    VerifyLayerLedExpectation(true);
    EXPECT_EQ(led_state_.GetBank1(), 0);
  }
}

TEST_F(LayerRTest, ShortcutIdPersistsAcrossModeChanges) {
  {
    EXPECT_TRUE(layer_r_.HandleEvent(Keypress::X));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank0(), 1);
  }
  EnterProgMode();
  {
    EXPECT_CALL(storage_controller_mock_, GetCharacterShortcut(2, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(layer_r_.HandleEvent(Keypress::Y));
    VerifyLayerLedExpectation(true);
    EXPECT_EQ(led_state_.GetBank1(), 2);
  }
  ExitProgMode();
  {
    EXPECT_TRUE(layer_r_.HandleEvent(Keypress::X));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank0(), 3);
  }
}

TEST_F(LayerRTest, ShortcutIdClear) {
  {
    EXPECT_TRUE(layer_r_.HandleEvent(Keypress::X));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank0(), 1);
  }
  {
    EXPECT_TRUE(layer_r_.HandleEvent(Keypress::XZ));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank0(), 0);
  }
}

TEST_F(LayerRTest, CommitShortcutToStorage) {
  // Increment the shortcut_id first.
  {
    EXPECT_TRUE(layer_r_.HandleEvent(Keypress::X));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank0(), 1);
  }
  EnterProgMode();
  {
    EXPECT_CALL(storage_controller_mock_, SetCharacterShortcut(1, _))
        .WillOnce(Return(true));
    EXPECT_CALL(storage_controller_mock_, GetCharacterShortcut(1, _))
        .WillOnce(Return(true));
    EXPECT_TRUE(layer_r_.HandleEvent(Keypress::XZ));
  }
}

TEST_F(LayerRTest, CurrentProgCharIncrement) {
  EnterProgMode(123);
  EXPECT_CALL(storage_controller_mock_, SetCharacterShortcut(0, 124))
      .WillOnce(Return(true));
  EXPECT_CALL(storage_controller_mock_, GetCharacterShortcut(0, _))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_r_.HandleEvent(Keypress::X));
  VerifyLayerLedExpectation(true);
}

TEST_F(LayerRTest, ModcodeIncrement) {
  EXPECT_TRUE(layer_r_.HandleEvent(Keypress::Y));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetBank0(), 0);
  EXPECT_EQ(led_state_.GetBank1(), 1);
}

TEST_F(LayerRTest, ModcodeClear) {
  {
    EXPECT_TRUE(layer_r_.HandleEvent(Keypress::Y));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank1(), 1);
  }
  {
    EXPECT_TRUE(layer_r_.HandleEvent(Keypress::YZ));
    VerifyLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank1(), 0);
  }
}

TEST_F(LayerRTest, UsbFlush) {
  EXPECT_CALL(storage_controller_mock_, GetCharacterShortcut(0, _))
      .WillOnce(DoAll(SetArgPointee<1>(123), Return(true)));
  EXPECT_CALL(usb_controller_mock_, SendKeypress(123, 0))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_r_.HandleEvent(Keypress::Z));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetErr()->state, LedState::OFF);
}

TEST_F(LayerRTest, UsbFlushTransmissionFailure) {
  EXPECT_CALL(storage_controller_mock_, GetCharacterShortcut(0, _))
      .WillOnce(DoAll(SetArgPointee<1>(123), Return(true)));
  EXPECT_CALL(usb_controller_mock_, SendKeypress(123, 0))
      .WillOnce(Return(false));
  EXPECT_TRUE(layer_r_.HandleEvent(Keypress::Z));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetErr()->state, LedState::ON);
}

TEST_F(LayerRTest, UsbFlushStorageFailure) {
  EXPECT_CALL(storage_controller_mock_, GetCharacterShortcut(0, _))
      .WillOnce(DoAll(SetArgPointee<1>(123), Return(false)));
  EXPECT_CALL(usb_controller_mock_, SendKeypress(123, 0)).Times(0);
  EXPECT_FALSE(layer_r_.HandleEvent(Keypress::Z));
}

TEST_F(LayerRTest, EnterProgMode) {
  EXPECT_CALL(storage_controller_mock_, GetCharacterShortcut(0, _))
      .WillOnce(DoAll(SetArgPointee<1>(123), Return(true)));
  EXPECT_TRUE(layer_r_.HandleEvent(Keypress::XY));
  VerifyLayerLedExpectation(true);
  EXPECT_EQ(led_state_.GetBank0(), 123);
}

TEST_F(LayerRTest, ExitProgMode) {
  EnterProgMode();
  EXPECT_TRUE(layer_r_.HandleEvent(Keypress::XYZ));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetProg()->state, LedState::OFF);
}

TEST_F(LayerRTest, LayerSwitch) {
  EXPECT_CALL(layer_controller_delegate_mock_, SwitchToLayer(LayerId::G))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_r_.HandleEvent(Keypress::XYZ));
}

TEST_F(LayerRTest, LayerSwitchFailure) {
  EXPECT_CALL(layer_controller_delegate_mock_, SwitchToLayer(LayerId::G))
      .WillOnce(Return(false));
  EXPECT_FALSE(layer_r_.HandleEvent(Keypress::XYZ));
}

TEST_F(LayerRTest, TransitionToLayer) {
  EXPECT_TRUE(layer_r_.TransitionedToLayer());
  VerifyLayerLedExpectation();
}

}  // namespace
}  // namespace threeboard