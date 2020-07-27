#include "key_controller.h"

#include "src/util/util.h"

namespace threeboard {
namespace {

constexpr uint8_t kXIndex = 3;
constexpr uint8_t kYIndex = 4;
constexpr uint8_t kZIndex = 5;

constexpr bool is_pressed(const uint8_t pin_register, const uint8_t idx) {
  return !(pin_register & (1 << idx));
}

constexpr bool was_pressed(const uint8_t state, const uint8_t offset) {
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
  // Initial state of the key mask is empty.
  key_mask_ = 0;
  // Set pins B1-B3 as input pins.
  native_->DisableDDRB(0b00001110);
  // Enable internal pullup resistors for B1-B3.
  native_->EnablePORTB(0b00001110);
}

void KeyController::PollKeyState() {
  uint8_t pinb = native_->GetPINB();
  // Key X.
  if (is_pressed(pinb, native::PB2)) {
    key_mask_ |= (1 << kXIndex);
  } else if (was_pressed(key_mask_, kXIndex)) {
    key_mask_ &= ~(1 << kXIndex);
    key_mask_ |= (uint8_t)Keypress::X;
  }
  // Key Y.
  if (is_pressed(pinb, native::PB3)) {
    key_mask_ |= (1 << kYIndex);
  } else if (was_pressed(key_mask_, kYIndex)) {
    key_mask_ &= ~(1 << kYIndex);
    key_mask_ |= (uint8_t)Keypress::Y;
  }
  // Key Z.
  if (is_pressed(pinb, native::PB1)) {
    key_mask_ |= (1 << kZIndex);
  } else if (was_pressed(key_mask_, kZIndex)) {
    key_mask_ &= ~(1 << kZIndex);
    key_mask_ |= (uint8_t)Keypress::Z;
  }

  // If there are no active keypresses but there were previous keypresses, a
  // keypress event should be registered.
  if ((key_mask_ >> 3) == 0 && key_mask_ > 0) {
    keypress_handler_->HandleKeypress((Keypress)(key_mask_ & 7));
    key_mask_ = 0;
  }
}

bool KeyController::HasActiveKeypress() {
  const uint8_t keymask =
      (1 << native::PB1) | (1 << native::PB2) | (1 << native::PB3);
  return native_->GetPINB() & keymask;
}
} // namespace threeboard
