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

  void EnterProgMode(int character = 0) {
    EXPECT_CALL(storage_controller_mock_, GetCharacterShortcut(_, _))
        .WillOnce(DoAll(SetArgPointee<1>(character), Return(true)));
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
  EXPECT_CALL(storage_controller_mock_, GetWordShortcutLength(_, _))
      .WillOnce(Return(true));
  EXPECT_TRUE(layer_g_.HandleEvent(Keypress::X));
  VerifyLayerLedExpectation();
  EXPECT_EQ(led_state_.GetBank0(), 1);
  EXPECT_EQ(led_state_.GetBank1(), 0);
}

}  // namespace
}  // namespace threeboard