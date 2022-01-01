#pragma once

#include "integration/model/layer_model.h"
#include "simulator/simulator_state.h"
#include "src/keypress.h"
#include "src/layers/layer_id.h"

namespace threeboard {
namespace integration {

// A model of the state of the threeboard. This class can receive keypresses and
// produces the correct expected state of the threeboard given those keypresses.
// It's a simple input/output model, it doesn't include anywhere near the
// complexity of the real threeboard firmware. It's used to verify that the real
// simulated threeboard firmware under test is always in the correct state.
class ThreeboardModel {
 public:
  ThreeboardModel() : current_layer_(LayerId::DFLT) {}

  void Apply(const Keypress& keypress);
  simulator::DeviceState GetStateSnapshot();

 private:
  LayerModel* CurrentLayerModel();

  DefaultLayerModel dflt_layer_model_;
  LayerRModel r_layer_model_;
  LayerGModel g_layer_model_;
  LayerBModel b_layer_model_;
  LayerId current_layer_;
};
}  // namespace integration
}  // namespace threeboard