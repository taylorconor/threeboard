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

bool DefaultLayerModel::Apply(const Keypress& keypress) {
  if (keypress == Keypress::X) {
    device_state_.bank_0++;
  } else if (keypress == Keypress::Y) {
    device_state_.bank_1++;
  } else if (keypress == Keypress::Z) {
    *usb_buffer_ +=
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

simulator::DeviceState DefaultLayerModel::GetStateSnapshot() {
  return device_state_;
}
}  // namespace integration
}  // namespace threeboard