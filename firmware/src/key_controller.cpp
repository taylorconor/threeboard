#include "key_controller.h"

namespace threeboard {
namespace {

#define STATE_OFFSET 13
#define X_IDX 3
#define Y_IDX 4
#define Z_IDX 5

__always_inline bool is_pressed(const uint8_t pin_register, const uint8_t idx) {
  return !(pin_register & (1 << idx));
}

__always_inline bool was_pressed(const uint8_t state, const uint8_t offset) {
  return state & (1 << offset);
}

} // namespace

// TODO: This is a very naive implementation of a keyboard controller. It lacks:
// - Debouncing (it assumes Cherry MX switches with 5ms debounce profile)
// - Key repeat
// - Event handling outside the ISR
// - More intelligent key combo registration

KeyController::KeyController(native::Native *native,
                             KeypressHandlerDelegate *keypress_handler)
    : native_(native), keypress_handler_(keypress_handler) {
  // Set pins B1-B3 as input pins.
  native_->DisableDDRB(0b00001110);
  // Enable internal pullup resistors for B1-B3.
  native_->EnablePORTB(0b00001110);
}

void KeyController::PollKeyState() {
  uint8_t pinb = native_->GetPINB();
  // Key X.
  if (is_pressed(pinb, native::PB1)) {
    key_mask_ |= (1 << X_IDX);
  } else if (was_pressed(key_mask_, X_IDX)) {
    key_mask_ &= ~(1 << X_IDX);
    key_mask_ |= key::X;
  }
  // Key Y.
  if (is_pressed(pinb, native::PB2)) {
    key_mask_ |= (1 << Y_IDX);
  } else if (was_pressed(key_mask_, Y_IDX)) {
    key_mask_ &= ~(1 << Y_IDX);
    key_mask_ |= key::Y;
  }
  // Key Z.
  if (is_pressed(pinb, native::PB3)) {
    key_mask_ |= (1 << Z_IDX);
  } else if (was_pressed(key_mask_, Z_IDX)) {
    key_mask_ &= ~(1 << Z_IDX);
    key_mask_ |= key::Z;
  }

  // If there are no active keypresses but there were previous keypresses, a
  // keypress event should be registered.
  if ((key_mask_ >> 3) == 0 && key_mask_ > 0) {
    keypress_handler_->HandleKeypress(key_mask_);
    key_mask_ = 0;
  }
}

bool KeyController::HasActiveKeypress() {
  const uint8_t keymask =
      (1 << native::PB1) | (1 << native::PB2) | (1 << native::PB3);
  return native_->GetPINB() & keymask;
}
} // namespace threeboard
