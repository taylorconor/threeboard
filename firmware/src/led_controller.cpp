#include "src/led_controller.h"

namespace threeboard {

/**
   Pin mappings:
   ERR (status1) - 31 - PC6
   STATUS (status0) - 30 - PB6
   row0 - 27 - PD7
   row1 - 28 - PB4
   row2 - 26 - PD6
   row3 - 25 - PD4
   row4 - 29 - PB5
   col0 - 41 - PF0
   col1 - 40 - PF1
   col2 - 39 - PF4
   col3 - 38 - PF5
 */

LedController::LedController(native::Native *native) : native_(native) {
  // Specify which pins will be used by this controller.
  native_->EnableDDRB(0b01110000);
  native_->EnableDDRC(0b01000000);
  native_->EnableDDRD(0b11010000);
  native_->EnableDDRF(0b00110011);
}

void LedController::ScanNextLine() {
  // The scan line identifies which row (or "line") of LEDs is next to be
  // refreshed. It is incremented on each scan.
  uint8_t scan_line = next_scan_line_;

  // Split this add and mod into separate operations to avoid a 16-bit divide
  // instruction (which I suspect is a GCC bug).
  next_scan_line_ += 1;
  next_scan_line_ %= 5;

  WriteStateToPins(scan_line);
}

void LedController::UpdateBlinkStatus() {
  // This is called every 5ms. Bit 7 of blink_status_ determines whether an LED
  // in BLINK state should be lit or not.
  blink_status_++;
  if (blink_status_ & (1 << 6)) {
    // Set bit 6 to 0 to reset the counter.
    blink_status_ &= ~(1 << 6);
    // Toggle bit 7, which indicates the blink state (on or off).
    blink_status_ ^= (1 << 7);
  }
}

void LedController::WriteStateToPins(uint8_t row) {
  // ERR and STATUS are a special case since they're mutually exclusive LEDs.
  // They could be set on each scan, but to maintain consistent brightness
  // they have each been assigned their own scan line.
  if (row == 0) {
    ApplyLedState(&native::Native::EnablePORTB, 1 << native::PB6,
                  state_.GetStatus());
  } else if (row == 1) {
    // STATUS must be disabled to allow ERR to be enabled.
    native_->DisablePORTB(1 << native::PB6);
    ApplyLedState(&native::Native::EnablePORTC, 1 << native::PC6,
                  state_.GetErr());
  } else if (row == 2) {
    // Disable both ERR and STATUS until the row scanner starts again.
    native_->DisablePORTC(1 << native::PC6);
  }

  // Clear all row and column pins first. The column pins in PORTF should be
  // considered as active low (they need to be grounded to enable the LED).
  native_->DisablePORTB(0b00110000);
  native_->DisablePORTD(0b11010000);
  native_->EnablePORTF(0b00110011);

  // Enable the correct row pin and column pins for the current row.
  if (row == 0) {
    // row 0: 4 MSB of bank 0
    WriteColumns(state_.GetBank0() >> 4);
    native_->EnablePORTD(1 << native::PD7);
  } else if (row == 1) {
    // row 1: 4 LSB of bank 0
    WriteColumns(state_.GetBank0());
    native_->EnablePORTB(1 << native::PB4);
  } else if (row == 2) {
    // row 2: 4 MSB of bank 1
    WriteColumns(state_.GetBank1() >> 4);
    native_->EnablePORTD(1 << native::PD6);
  } else if (row == 3) {
    // row 3: 4 LSB of bank 1
    WriteColumns(state_.GetBank1());
    native_->EnablePORTD(1 << native::PD4);
  } else if (row == 4) {
    // row 4: R,G,B and PROG. These support full LedState rather than simple
    // booleans.
    ApplyLedState(&native::Native::DisablePORTF, 1 << native::PF0,
                  state_.GetR());
    ApplyLedState(&native::Native::DisablePORTF, 1 << native::PF1,
                  state_.GetG());
    ApplyLedState(&native::Native::DisablePORTF, 1 << native::PF4,
                  state_.GetB());
    ApplyLedState(&native::Native::DisablePORTF, 1 << native::PF5,
                  state_.GetProg());
    native_->EnablePORTB(1 << native::PB5);
  }
}

void LedController::WriteColumns(uint8_t vals) {
  if (vals & 8) {
    native_->DisablePORTF(1 << native::PF0);
  }
  if (vals & 4) {
    native_->DisablePORTF(1 << native::PF1);
  }
  if (vals & 2) {
    native_->DisablePORTF(1 << native::PF4);
  }
  if (vals & 1) {
    native_->DisablePORTF(1 << native::PF5);
  }
}

bool LedController::ShouldEnableBlinkingLed() const {
  return blink_status_ & (1 << 7);
}

void LedController::ApplyLedState(native::PortModFn port_mod_fn, uint8_t val,
                                  LedState::FullState *state) {
  if (state->state == LedState::ON ||
      (state->state == LedState::BLINK && ShouldEnableBlinkingLed())) {
    (native_->*port_mod_fn)(val);
  }
  if (state->state == LedState::PULSE) {
    state->pulse_timer++;
    if (state->pulse_timer == 0) {
      // When the pulse timer has elapsed, return the LED state to OFF.
      state->state = LedState::OFF;
    } else {
      (native_->*port_mod_fn)(val);
    }
  }
}

}  // namespace threeboard
