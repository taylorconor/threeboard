#include "src/layers/default_layer.h"

#include "gmock/gmock.h"
#include "src/delegates/layer_controller_delegate_mock.h"
#include "src/logging_fake.h"
#include "src/usb/usb_controller_mock.h"

namespace threeboard {
namespace {

using testing::Return;

class DefaultLayerTest : public ::testing::Test {
 public:
  DefaultLayerTest()
      : default_layer_(&led_state_, &usb_controller_mock_,
                       &layer_controller_delegate_mock_) {}

  void SetLayerLedExpectation() {
    EXPECT_EQ(led_state_.GetR()->state, LedState::OFF);
    EXPECT_EQ(led_state_.GetG()->state, LedState::OFF);
    EXPECT_EQ(led_state_.GetB()->state, LedState::OFF);
  }

  LoggingFake logging_fake_;
  LedState led_state_;
  usb::UsbControllerMock usb_controller_mock_;
  LayerControllerDelegateMock layer_controller_delegate_mock_;
  DefaultLayer default_layer_;
};

TEST_F(DefaultLayerTest, Bank0Increment) {
  EXPECT_TRUE(default_layer_.HandleEvent(Keypress::X));
  SetLayerLedExpectation();
  EXPECT_EQ(led_state_.GetBank0(), 1);
  EXPECT_EQ(led_state_.GetBank1(), 0);
}

TEST_F(DefaultLayerTest, Bank0Clear) {
  {
    EXPECT_TRUE(default_layer_.HandleEvent(Keypress::X));
    SetLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank0(), 1);
  }
  {
    EXPECT_TRUE(default_layer_.HandleEvent(Keypress::XZ));
    SetLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank0(), 0);
  }
}

TEST_F(DefaultLayerTest, Bank1Increment) {
  EXPECT_TRUE(default_layer_.HandleEvent(Keypress::Y));
  SetLayerLedExpectation();
  EXPECT_EQ(led_state_.GetBank0(), 0);
  EXPECT_EQ(led_state_.GetBank1(), 1);
}

TEST_F(DefaultLayerTest, Bank1Clear) {
  {
    EXPECT_TRUE(default_layer_.HandleEvent(Keypress::Y));
    SetLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank1(), 1);
  }
  {
    EXPECT_TRUE(default_layer_.HandleEvent(Keypress::YZ));
    SetLayerLedExpectation();
    EXPECT_EQ(led_state_.GetBank1(), 0);
  }
}

TEST_F(DefaultLayerTest, UsbFlush) {
  EXPECT_CALL(usb_controller_mock_, SendKeypress(0, 0)).WillOnce(Return(true));
  EXPECT_TRUE(default_layer_.HandleEvent(Keypress::Z));
  SetLayerLedExpectation();
  EXPECT_EQ(led_state_.GetErr()->state, LedState::OFF);
}

TEST_F(DefaultLayerTest, UsbFlushFailure) {
  EXPECT_CALL(usb_controller_mock_, SendKeypress(0, 0)).WillOnce(Return(false));
  EXPECT_TRUE(default_layer_.HandleEvent(Keypress::Z));
  SetLayerLedExpectation();
  EXPECT_EQ(led_state_.GetErr()->state, LedState::ON);
}

TEST_F(DefaultLayerTest, LayerSwitch) {
  EXPECT_CALL(layer_controller_delegate_mock_, SwitchToLayer(LayerId::R))
      .WillOnce(Return(true));
  EXPECT_TRUE(default_layer_.HandleEvent(Keypress::XYZ));
}

TEST_F(DefaultLayerTest, LayerSwitchFailure) {
  EXPECT_CALL(layer_controller_delegate_mock_, SwitchToLayer(LayerId::R))
      .WillOnce(Return(false));
  EXPECT_FALSE(default_layer_.HandleEvent(Keypress::XYZ));
}
}  // namespace
}  // namespace threeboard
