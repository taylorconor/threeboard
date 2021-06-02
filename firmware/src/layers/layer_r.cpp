#include "src/layers/layer_r.h"

#include "src/logging.h"

namespace threeboard {

void LayerR::HandleEvent(const Keypress &keypress) {
  if (keypress == Keypress::X) {
    if (prog_) {
      storage_controller_->SetCharacterShortcut(
          shortcut_id_,
          storage_controller_->GetCharacterShortcut(shortcut_id_) + 1);
    } else {
      shortcut_id_++;
    }
  } else if (keypress == Keypress::Y) {
    if (prog_) {
      shortcut_id_++;
    } else {
      modcode_++;
    }
  } else if (keypress == Keypress::Z) {
    if (!prog_) {
      SendToHost(storage_controller_->GetCharacterShortcut(shortcut_id_),
                 modcode_);
    }
  } else if (keypress == Keypress::XY) {
    prog_ = true;
  } else if (keypress == Keypress::XZ) {
    if (prog_) {
      storage_controller_->SetCharacterShortcut(shortcut_id_, 0);
    } else {
      shortcut_id_ = 0;
    }
  } else if (keypress == Keypress::YZ) {
    if (prog_) {
      shortcut_id_ = 0;
    } else {
      modcode_ = 0;
    }
  } else if (keypress == Keypress::XYZ) {
    if (prog_) {
      prog_ = false;
    } else {
      layer_controller_delegate_->SwitchToLayer(LayerId::G);
      return;
    }
  }
  if (prog_) {
    UpdateLedState(LayerId::R,
                   storage_controller_->GetCharacterShortcut(shortcut_id_),
                   shortcut_id_);
  } else {
    UpdateLedState(LayerId::R, shortcut_id_, modcode_);
  }
}

void LayerR::TransitionedToLayer() {
  LOG("Switched to layer R");
  UpdateLedState(LayerId::R, shortcut_id_, modcode_);
}

}  // namespace threeboard