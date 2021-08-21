#include "src/layers/layer_controller.h"

#include <memory>

#include "src/layers/layer_mock.h"

namespace threeboard {
namespace {

class TestableLayerController : public LayerController {
 public:
  TestableLayerController(Layer *dflt, Layer *r, Layer *g, Layer *b)
      : LayerController(dflt, r, g, b) {}
};

using testing::Return;

class LayerControllerTest : public ::testing::Test {
 public:
  LayerControllerTest()
      : layer_controller_(std::make_unique<TestableLayerController>(
            &mock_layer_dflt_, &mock_layer_r_, &mock_layer_g_,
            &mock_layer_b_)) {}

  LayerMock mock_layer_dflt_;
  LayerMock mock_layer_r_;
  LayerMock mock_layer_g_;
  LayerMock mock_layer_b_;
  std::unique_ptr<LayerController> layer_controller_;
};

TEST_F(LayerControllerTest, DefaultInitialLayer) {
  Keypress event = Keypress::X;
  EXPECT_CALL(mock_layer_dflt_, HandleEvent(event)).WillOnce(Return(true));
  EXPECT_TRUE(layer_controller_->HandleEvent(event));
}

TEST_F(LayerControllerTest, SwitchToLayer) {
  Keypress event = Keypress::X;
  // First verify that DFLT handles the event.
  {
    EXPECT_CALL(mock_layer_dflt_, HandleEvent(event)).WillOnce(Return(true));
    EXPECT_TRUE(layer_controller_->HandleEvent(event));
  }
  // Switch to layer R.
  {
    EXPECT_CALL(mock_layer_r_, TransitionedToLayer).WillOnce(Return(true));
    EXPECT_TRUE(layer_controller_->SwitchToLayer(LayerId::R));
  }
  // Now verify that layer R handles the event.
  {
    EXPECT_CALL(mock_layer_r_, HandleEvent(event)).WillOnce(Return(true));
    EXPECT_TRUE(layer_controller_->HandleEvent(event));
  }
}

TEST_F(LayerControllerTest, PropagateFailureStatus) {
  {
    EXPECT_CALL(mock_layer_r_, TransitionedToLayer).WillOnce(Return(false));
    EXPECT_FALSE(layer_controller_->SwitchToLayer(LayerId::R));
  }
  {
    Keypress event = Keypress::X;
    EXPECT_CALL(mock_layer_r_, HandleEvent(event)).WillOnce(Return(false));
    EXPECT_FALSE(layer_controller_->HandleEvent(event));
  }
}
}  // namespace
}  // namespace threeboard
