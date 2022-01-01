#include "integration/model/layer_model.h"

#include "simulator/components/usb_keycodes.h"
#include "src/storage/storage_controller.h"

namespace threeboard {
namespace integration {
namespace {

using storage::WordModCode;

void AppendIfPrintable(std::string* str, char c) {
  // We don't want to append a character to the output string if it's not
  // printable because it messes up the comparison between the model and the
  // simulator.
  if (std::isprint(c)) {
    *str += c;
  }
}

void AppendTo(uint8_t mod_code, uint8_t key_code, std::vector<char>* vec) {
  vec->push_back(simulator::FromUsbKeycodes(key_code, mod_code));
}

void AppendTo(uint8_t mod_code, uint8_t key_code, std::string* s) {
  char c = simulator::FromUsbKeycodes(key_code, mod_code);
  AppendIfPrintable(s, c);
}

}  // namespace

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

simulator::DeviceState DefaultLayerModel::GetStateSnapshot() {
  simulator::DeviceState snapshot = device_state_;
  device_state_.usb_buffer = "";
  return snapshot;
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

bool LayerBModel::Apply(const Keypress& keypress) {
  if (keypress == Keypress::X) {
    if (prog_) {
      key_code_++;
    } else {
      shortcut_id_++;
    }
  } else if (keypress == Keypress::Y) {
    if (prog_) {
      mod_code_++;
    }
  } else if (keypress == Keypress::Z) {
    if (prog_) {
      AppendTo(mod_code_, key_code_, &shortcuts_[shortcut_id_]);
    } else {
      for (const char& c : shortcuts_[shortcut_id_]) {
        AppendIfPrintable(&usb_buffer_, c);
      }
    }
  } else if (keypress == Keypress::XY) {
    if (prog_) {
      shortcuts_[shortcut_id_].clear();
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
      mod_code_ = 0;
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

simulator::DeviceState LayerBModel::GetStateSnapshot() {
  simulator::DeviceState snapshot;
  if (prog_) {
    snapshot.bank_0 = key_code_;
    snapshot.bank_1 = mod_code_;
  } else {
    snapshot.bank_0 = shortcut_id_;
    snapshot.bank_1 = shortcuts_[shortcut_id_].size();
  }
  snapshot.led_b = true;
  snapshot.led_prog = prog_;
  snapshot.usb_buffer = usb_buffer_;
  usb_buffer_ = "";
  return snapshot;
}
}  // namespace integration
}  // namespace threeboard