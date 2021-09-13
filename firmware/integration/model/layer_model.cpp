#include "integration/model/layer_model.h"

namespace threeboard {
namespace integration {
namespace {

std::string CreateAsciiString(uint8_t keycode, uint8_t modcode) {
  bool capitalise = false;
  // Check for L_SHIFT and R_SHIFT.
  if ((modcode & 0x22) > 0 && (modcode & ~0x22) == 0) {
    capitalise = true;
  } else if (modcode != 0) {
    // Ignore and reject any non-shift modcodes.
    return "";
  }
  char c;
  if (keycode >= 0x04 && keycode <= 0x1d) {
    c = keycode + 0x5d;
    if (capitalise) {
      c -= 0x20;
    }
  } else if (keycode == 0x2a) {
    c = ' ';
  } else if (keycode == 0x2d) {
    c = '-';
  } else if (keycode == 0x36) {
    c = ',';
  } else if (keycode == 0x37) {
    c = '.';
  } else {
    // Ignore unsupported characters.
    return "";
  }
  return std::string(1, c);
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
    device_state_.usb_buffer +=
        CreateAsciiString(device_state_.bank_0, device_state_.bank_1);
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
      usb_buffer_ += CreateAsciiString(shortcuts_[shortcut_id_], modcode_);
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
}  // namespace integration
}  // namespace threeboard