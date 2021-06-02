#include "src/layers/default_layer.h"

#include "src/logging.h"

namespace threeboard {

void DefaultLayer::HandleEvent(const Keypress &keypress) {
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
    layer_controller_delegate_->SwitchToLayer(LayerId::R);
    return;
  }
  UpdateLedState(LayerId::DFLT, bank0_, bank1_);
}

void DefaultLayer::TransitionedToLayer() {
  LOG("Switched to layer DFLT");
  UpdateLedState(LayerId::DFLT, bank0_, bank1_);
}

}  // namespace threeboard