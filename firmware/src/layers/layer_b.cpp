#include "src/layers/layer_b.h"

#include "src/logging.h"

namespace threeboard {

bool LayerB::HandleEvent(const Keypress &keypress) {
  if (keypress == Keypress::X) {
    if (prog_) {
      key_code_++;
    } else {
      shortcut_id_++;
    }
  } else if (keypress == Keypress::Y) {
    if (prog_) {
      modcode_++;
    }
  } else if (keypress == Keypress::Z) {
    if (prog_) {
      storage_controller_->AppendToBlobShortcut(shortcut_id_, key_code_,
                                                modcode_);
    } else {
      storage_controller_->SendBlobShortcut(shortcut_id_);
    }
  } else if (keypress == Keypress::XY) {
    if (prog_) {
      storage_controller_->ClearBlobShortcut(shortcut_id_);
    } else {
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
      modcode_ = 0;
    }
  } else if (keypress == Keypress::XYZ) {
    if (prog_) {
      prog_ = false;
    } else {
      return layer_controller_delegate_->SwitchToLayer(LayerId::DFLT);
    }
  }
  if (prog_) {
    UpdateLedState(LayerId::B, key_code_, modcode_);
  } else {
    uint8_t length;
    RETURN_IF_ERROR(
        storage_controller_->GetBlobShortcutLength(shortcut_id_, &length));
    UpdateLedState(LayerId::B, shortcut_id_, length);
  }
  return true;
}

bool LayerB::TransitionedToLayer() {
  LOG("Switched to layer B");
  uint8_t length;
  RETURN_IF_ERROR(
      storage_controller_->GetBlobShortcutLength(shortcut_id_, &length));
  UpdateLedState(LayerId::B, shortcut_id_, length);
  return true;
}

}  // namespace threeboard