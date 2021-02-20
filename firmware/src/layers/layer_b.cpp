#include "src/layers/layer_b.h"

#include "src/logging.h"

namespace threeboard {

void LayerB::HandleEvent(const Keypress &keypress) {
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
    layer_controller_delegate_->SwitchToLayer(LayerId::DFLT);
    return;
  }
  UpdateLedState(LayerId::B);
}

void LayerB::TransitionedToLayer() {
  LOG("Switched to layer B");
  UpdateLedState(LayerId::B);
}

}  // namespace threeboard