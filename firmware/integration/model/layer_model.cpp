#include "integration/model/layer_model.h"

#include "src/storage/storage_controller.h"

namespace threeboard {
namespace integration {
namespace {

using storage::WordModCode;

template <typename T>
void AppendTo(uint8_t mod_code, uint8_t key_code, T* vec) {
  bool capitalise = false;
  // Check for L_SHIFT and R_SHIFT.
  if ((mod_code & 0x22) > 0 && (mod_code & ~0x22) == 0) {
    capitalise = true;
  }
  char c = 0;
  if (key_code >= 0x04 && key_code <= 0x1d) {
    c = key_code + 0x5d;
    if (capitalise) {
      c -= 0x20;
    }
  } else if (key_code == 0x2a) {
    c = ' ';
  } else if (key_code == 0x2d) {
    c = '-';
  } else if (key_code == 0x36) {
    c = ',';
  } else if (key_code == 0x37) {
    c = '.';
  }
  if (std::is_same<T, std::string>::value && !std::isprint(c)) {
    return;
  }
  vec->push_back(c);
}

void AppendIfPrintable(std::string* str, char c) {
  if (std::isprint(c)) {
    *str += c;
  }
}

}  // namespace

simulator::DeviceState LayerModel::GetStateSnapshot() {
  simulator::DeviceState snapshot = device_state_;
  device_state_.usb_buffer = "";
  return snapshot;
}

bool DefaultLayerModel::Apply(const Keypress& keypress) {
  if (keypress == Keypress::X) {
    device_state_.bank_0++;
  } else if (keypress == Keypress::Y) {
    device_state_.bank_1++;
  } else if (keypress == Keypress::Z) {
    if (device_state_.bank_0 != 0 || device_state_.bank_1 != 0) {
      AppendTo(device_state_.bank_1, device_state_.bank_0,
               &device_state_.usb_buffer);
    }
  } else if (keypress == Keypress::XZ) {
    device_state_.bank_0 = 0;
  } else if (keypress == Keypress::YZ) {
    device_state_.bank_1 = 0;
  } else if (keypress == Keypress::XYZ) {
    return true;
  }
  return false;
}

bool LayerRModel::Apply(const Keypress& keypress) {
  if (keypress == Keypress::X) {
    if (prog_) {
      shortcuts_[shortcut_id_]++;
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
      if (modcode_ != 0 || shortcuts_[shortcut_id_] != 0) {
        AppendTo(modcode_, shortcuts_[shortcut_id_], &usb_buffer_);
      }
    }
  } else if (keypress == Keypress::XY) {
    prog_ = true;
  } else if (keypress == Keypress::XZ) {
    if (prog_) {
      shortcuts_[shortcut_id_] = 0;
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
      return true;
    }
  }
  return false;
}

simulator::DeviceState LayerRModel::GetStateSnapshot() {
  simulator::DeviceState snapshot;
  if (prog_) {
    snapshot.bank_0 = shortcuts_[shortcut_id_];
    snapshot.bank_1 = shortcut_id_;
  } else {
    snapshot.bank_0 = shortcut_id_;
    snapshot.bank_1 = modcode_;
  }
  snapshot.led_r = true;
  snapshot.led_prog = prog_;
  snapshot.usb_buffer = usb_buffer_;
  usb_buffer_ = "";
  return snapshot;
}

bool LayerGModel::Apply(const Keypress& keypress) {
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
    if (prog_ && shortcuts_[shortcut_id_].size() < 15) {
      AppendTo(0, key_code_, &shortcuts_[shortcut_id_]);
    } else {
      usb_buffer_ += ApplyModCodeToCurrentShortcut();
    }
  } else if (keypress == Keypress::XY) {
    prog_ = true;
  } else if (keypress == Keypress::XZ) {
    if (prog_) {
      key_code_ = 0;
    } else {
      shortcut_id_ = 0;
    }
  } else if (keypress == Keypress::YZ) {
    if (prog_) {
      shortcuts_[shortcut_id_].clear();
    } else {
      word_mod_code_ = 0;
    }
  } else if (keypress == Keypress::XYZ) {
    if (prog_) {
      prog_ = false;
    } else {
      return true;
    }
  }
  return false;
}

simulator::DeviceState LayerGModel::GetStateSnapshot() {
  simulator::DeviceState snapshot;
  if (prog_) {
    snapshot.bank_0 = key_code_;
    snapshot.bank_1 = shortcuts_[shortcut_id_].size() << 4;
  } else {
    snapshot.bank_0 = shortcut_id_;
    snapshot.bank_1 = word_mod_code_ | (shortcuts_[shortcut_id_].size() << 4);
  }
  snapshot.led_g = true;
  snapshot.led_prog = prog_;
  snapshot.usb_buffer = usb_buffer_;
  usb_buffer_ = "";
  return snapshot;
}

std::string LayerGModel::ApplyModCodeToCurrentShortcut() {
  std::string output;
  for (int i = 0; i < shortcuts_[shortcut_id_].size(); ++i) {
    char c = shortcuts_[shortcut_id_].at(i);
    if (word_mod_code_ == (int)WordModCode::UPPERCASE ||
        (word_mod_code_ == (int)WordModCode::CAPITALISE && i == 0)) {
      AppendIfPrintable(&output, (char)toupper(c));
    } else {
      if (i == shortcuts_[shortcut_id_].size() - 1) {
        char append = 0;
        if (word_mod_code_ == (int)WordModCode::APPEND_PERIOD) {
          append = '.';
        } else if (word_mod_code_ == (int)WordModCode::APPEND_COMMA) {
          append = ',';
        } else if (word_mod_code_ == (int)WordModCode::APPEND_HYPHEN) {
          append = '-';
        }
        if (append > 0) {
          AppendIfPrintable(&output, c);
          c = append;
        }
      }
      AppendIfPrintable(&output, c);
    }
  }
  return output;
}
}  // namespace integration
}  // namespace threeboard