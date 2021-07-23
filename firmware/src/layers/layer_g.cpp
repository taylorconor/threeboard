#include "src/layers/layer_g.h"

#include "src/logging.h"

namespace threeboard {

bool LayerG::HandleEvent(const Keypress &keypress) {
  if (keypress == Keypress::X) {
    if (prog_) {
      key_code_++;
    } else {
      shortcut_id_++;
    }
  } else if (keypress == Keypress::Y) {
    if (!prog_) {
      word_mod_code_++;
    }
  } else if (keypress == Keypress::Z) {
    if (prog_) {
      // Append key_code.
      RETURN_IF_ERROR(
          storage_controller_->AppendToWordShortcut(shortcut_id_, key_code_));
    } else {
      RETURN_IF_ERROR(
          storage_controller_->SendWordShortcut(shortcut_id_, word_mod_code_));
    }
  } else if (keypress == Keypress::XY) {
    if (!prog_) {
      prog_ = true;
    }
  } else if (keypress == Keypress::XZ) {
    if (prog_) {
      key_code_ = 0;
    } else {
      shortcut_id_ = 0;
    }
  } else if (keypress == Keypress::YZ) {
    if (prog_) {
      RETURN_IF_ERROR(storage_controller_->ClearWordShortcut(shortcut_id_));
    } else {
      word_mod_code_ = 0;
    }
  } else if (keypress == Keypress::XYZ) {
    if (!prog_) {
      return layer_controller_delegate_->SwitchToLayer(LayerId::B);
    } else {
      prog_ = false;
    }
  }
  uint8_t length;
  RETURN_IF_ERROR(
      storage_controller_->GetWordShortcutLength(shortcut_id_, &length));
  if (prog_) {
    UpdateLedState(LayerId::G, key_code_, length << 4);
  } else {
    UpdateLedState(LayerId::G, shortcut_id_, (length << 4) | word_mod_code_);
  }
  return true;
}

bool LayerG::TransitionedToLayer() {
  LOG("Switched to layer G");
  uint8_t length;
  RETURN_IF_ERROR(
      storage_controller_->GetWordShortcutLength(shortcut_id_, &length));
  UpdateLedState(LayerId::G, shortcut_id_, (length << 4) | word_mod_code_);
  return true;
}

}  // namespace threeboard