#include "src/layers/layer_r.h"

#include "src/logging.h"

namespace threeboard {

bool LayerR::HandleEvent(const Keypress &keypress) {
  if (keypress == Keypress::X) {
    if (prog_) {
      storage_controller_->SetCharacterShortcut(shortcut_id_,
                                                current_prog_char_ + 1);
      // TODO: only increment here if the previous function call succeeded!
      current_prog_char_ += 1;
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
      uint8_t character = 0;
      storage_controller_->GetCharacterShortcut(shortcut_id_, &character);
      SendToHost(character, modcode_);
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
      return true;
    }
  }
  if (prog_) {
    uint8_t character = 0;
    storage_controller_->GetCharacterShortcut(shortcut_id_, &character);
    current_prog_char_ = character;
    UpdateLedState(LayerId::R, current_prog_char_, shortcut_id_);
  } else {
    UpdateLedState(LayerId::R, shortcut_id_, modcode_);
  }
  return true;
}

void LayerR::TransitionedToLayer() {
  LOG("Switched to layer R");
  UpdateLedState(LayerId::R, shortcut_id_, modcode_);
}

}  // namespace threeboard