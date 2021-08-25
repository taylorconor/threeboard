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
    bank0_++;
  } else if (keypress == Keypress::Y) {
    bank1_++;
  } else if (keypress == Keypress::Z) {
    *usb_buffer_ += CreateAsciiString(bank0_, bank1_);
  } else if (keypress == Keypress::XZ) {
    bank0_ = 0;
  } else if (keypress == Keypress::YZ) {
    bank1_ = 0;
  } else if (keypress == Keypress::XYZ) {
    return true;
  }
  return false;
}

ObservableState DefaultLayerModel::GetStateSnapshot() {
  ObservableState state;
  state.bank_0 = bank0_;
  state.bank_1 = bank1_;
  state.usb_buffer = *usb_buffer_;
  *usb_buffer_ = "";
  return state;
}
}  // namespace integration
}  // namespace threeboard