#pragma once

#include "gmock/gmock.h"
#include "src/delegates/layer_controller_delegate.h"

namespace threeboard {
class LayerControllerDelegateMockDefault : public LayerControllerDelegate {
 public:
  MOCK_METHOD(bool, SwitchToLayer, (const LayerId &), (override));
};

using LayerControllerDelegateMock =
    ::testing::StrictMock<LayerControllerDelegateMockDefault>;

}  // namespace threeboard
