#include "src/layers/layer_g.h"

#include "src/logging.h"

namespace threeboard {

void LayerG::HandleEvent(const Keypress &keypress) {
  if (keypress == Keypress::X) {
    bank0_++;
  } else if (keypress == Keypress::Y) {
    bank1_++;
  } else if (keypress == Keypress::Z) {
    SendToHost(bank0_, bank1_);
  } else if (keypress == Keypress::XZ) {
    bank0_ = 0;
  } else if (keypress == Keypress::YZ) {
    bank1_ = 0;
  } else if (keypress == Keypress::XYZ) {
    layer_controller_delegate_->SwitchToLayer(LayerId::B);
    return;
  }
  UpdateLedState(LayerId::G, bank0_, bank1_);
}

void LayerG::TransitionedToLayer() {
  LOG("Switched to layer G");
  UpdateLedState(LayerId::G, bank0_, bank1_);
}

}  // namespace threeboard