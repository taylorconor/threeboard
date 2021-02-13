#include "src/layers/layer_r.h"

#include "src/logging.h"

namespace threeboard {

void LayerR::HandleEvent(const Keypress &keypress) {
  if (keypress == Keypress::X) {
    bank0_++;
  } else if (keypress == Keypress::Y) {
    bank1_++;
  } else if (keypress == Keypress::Z) {
    FlushToHost();
  } else if (keypress == Keypress::XZ) {
    bank0_ = 0;
  } else if (keypress == Keypress::YZ) {
    bank1_ = 0;
  } else if (keypress == Keypress::XYZ) {
    layer_controller_delegate_->SwitchToLayer(LayerId::G);
    return;
  }
  UpdateLedState(LayerId::R);
}

void LayerR::TransitionedToLayer() {
  LOG("Switched to layer R");
  UpdateLedState(LayerId::R);
}

} // namespace threeboard