#include "src/led_controller.h"

namespace threeboard {
/**
   Pin mappings:
   STATUS (status1) - 31 - PC6
   ERR (status0) - 30 - PB6
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
  // refreshed. It occupies the last 3 bits in the bitset, and is incremented on
  // each scan.
  uint8_t scan_line = next_scan_line_;
  next_scan_line_ = (next_scan_line_ + 1) % 5;

  WriteStateToPins(scan_line);
}

void LedController::UpdateBlinkStatus() {
  // This is called every 5ms. Bit 7 of blink_status_ toggles every 640ms (used
  // to determine the blink status), and bit 6 toggles every 320ms (used to
  // determine the fast blink status). So this should wrap around by design.
  blink_status_++;
}

void LedController::SetBank0(uint8_t val) { bank_0_ = val; }
void LedController::SetBank1(uint8_t val) { bank_1_ = val; }
void LedController::SetR(LedState state) { led_r_ = state; }
void LedController::SetG(LedState state) { led_g_ = state; }
void LedController::SetB(LedState state) { led_b_ = state; }
void LedController::SetProg(LedState state) { led_prog_ = state; }
void LedController::SetErr(LedState state) { led_err_ = state; }
void LedController::SetStatus(LedState state) { led_status_ = state; }

void LedController::WriteStateToPins(uint8_t row) {
  // ERR and STATUS are a special case since they're mutually exclusive LEDs.
  // They could be set on each scan, but to maintain consistent brightness
  // they have each been assigned their own scan line.
  if (row == 0) {
    ApplyLedState(&native::Native::EnablePORTB, 1 << native::PB6, led_err_);
  } else if (row == 1) {
    // ERR must be disabled to allow STATUS to be enabled.
    native_->DisablePORTB(1 << native::PB6);
    ApplyLedState(&native::Native::EnablePORTC, 1 << native::PC6, led_status_);
  } else {
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
    native_->EnablePORTD(1 << native::PD7);
    WriteColumns(bank_0_ >> 4);
  } else if (row == 1) {
    // row 1: 4 LSB of bank 0
    native_->EnablePORTB(1 << native::PB4);
    WriteColumns(bank_0_ & 0xFF);
  } else if (row == 2) {
    // row 2: 4 MSB of bank 1
    native_->EnablePORTD(1 << native::PD6);
    WriteColumns(bank_1_ >> 4);
  } else if (row == 3) {
    // row 3: 4 LSB of bank 1
    native_->EnablePORTD(1 << native::PD4);
    WriteColumns(bank_1_ & 0xFF);
  } else if (row == 4) {
    // row 4: R,G,B and PROG. These support full LedState rather than simple
    // booleans.
    native_->EnablePORTB(1 << native::PB5);
    ApplyLedState(&native::Native::DisablePORTF, 1 << native::PF0, led_r_);
    ApplyLedState(&native::Native::DisablePORTF, 1 << native::PF1, led_g_);
    ApplyLedState(&native::Native::DisablePORTF, 1 << native::PF4, led_b_);
    ApplyLedState(&native::Native::DisablePORTF, 1 << native::PF5, led_prog_);
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

void LedController::ApplyLedState(native::PortModFn port_mod_fn, uint8_t val,
                                  LedState state) {
  if (state == LedState::ON) {
    (native_->*port_mod_fn)(val);
  } else if (state == LedState::BLINK) {
    if (blink_status_ & 0x80) {
      (native_->*port_mod_fn)(val);
    }
  } else if (state == LedState::BLINK_FAST) {
    if (blink_status_ & 0x40) {
      (native_->*port_mod_fn)(val);
    }
  }
}

} // namespace threeboard
