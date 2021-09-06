#include "threeboard_model.h"

namespace threeboard {
namespace integration {

void ThreeboardModel::Apply(const Keypress& keypress) {
  bool should_switch = CurrentLayerModel()->Apply(keypress);
  if (should_switch) {
    current_layer_ = (LayerId)((current_layer_ + 1) % 4);
  }
}

simulator::DeviceState ThreeboardModel::GetStateSnapshot() {
  return CurrentLayerModel()->GetStateSnapshot();
}

LayerModel* ThreeboardModel::CurrentLayerModel() {
  switch (current_layer_) {
    case LayerId::DFLT:
      return &dflt_layer_model_;
    case LayerId::R:
      return &r_layer_model_;
    case LayerId::G:
      return &g_layer_model_;
    case LayerId::B:
      return &b_layer_model_;
  }
}
}  // namespace integration
}  // namespace threeboard