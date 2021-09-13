#pragma once

#include "integration/model/layer_model.h"
#include "simulator/simulator_state.h"
#include "src/keypress.h"
#include "src/layers/layer_id.h"

namespace threeboard {
namespace integration {

class ThreeboardModel {
 public:
  ThreeboardModel() { current_layer_ = LayerId::DFLT; }

  void Apply(const Keypress& keypress);
  simulator::DeviceState GetStateSnapshot();

 private:
  LayerModel* CurrentLayerModel();

  DefaultLayerModel dflt_layer_model_;
  LayerRModel r_layer_model_;
  DefaultLayerModel g_layer_model_;
  DefaultLayerModel b_layer_model_;
  LayerId current_layer_;
};
}  // namespace integration
}  // namespace threeboard