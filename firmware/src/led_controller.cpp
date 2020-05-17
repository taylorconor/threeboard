#include "led_controller.h"

namespace threeboard {
namespace {

// defines indexes into the `state_` bitset.
constexpr uint8_t kBank0 = 0;
constexpr uint8_t kBank1 = 16;
constexpr uint8_t kLedR = 32;
constexpr uint8_t kLedG = 34;
constexpr uint8_t kLedB = 36;
constexpr uint8_t kLedProg = 38;
constexpr uint8_t kLedErr = 40;
constexpr uint8_t kLedStatus = 42;
constexpr uint8_t kBlinkStatus = 44;
constexpr uint8_t kPrevScanLine = 45;

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

} // namespace

LedController::LedController(native::Native *native) : native_(native) {
  // Specify which pins will be used by this controller.
  native_->EnableDDRB(0b01110000);
  native_->EnableDDRC(0b01000000);
  native_->EnableDDRD(0b11010000);
  native_->EnableDDRF(0b00110011);
}

void LedController::ScanNextLine() {
  // The scan line occupies the last 3 bits in the bitset. It should be
  // overwritten on each scan.
  uint8_t scan_line = state_.GetContainerByte(kPrevScanLine) & 7;
  WriteStateToPins(scan_line);
  state_.SetBits(kPrevScanLine, 3, (scan_line + 1) % 5);
}

void LedController::SetBank0(uint8_t val) { SetBankValue(kBank0, val); }
void LedController::SetBank1(uint8_t val) { SetBankValue(kBank1, val); }
void LedController::SetR(LedState state) { SetLedState(kLedR, state); }
void LedController::SetG(LedState state) { SetLedState(kLedG, state); }
void LedController::SetB(LedState state) { SetLedState(kLedB, state); }
void LedController::SetProg(LedState state) { SetLedState(kLedProg, state); }
void LedController::SetErr(LedState state) { SetLedState(kLedErr, state); }
void LedController::SetStatus(LedState state) {
  SetLedState(kLedStatus, state);
}

void LedController::WriteStateToPins(uint8_t row) {
  // ERR and STATUS are a special case since they're mutally exclusive LEDs.
  // They could be set on each scan, but to maintain consistent brightness they
  // have each been assigned their own scan line.
  // TODO: uncomment after testing.
  if (row == 0) {
    if (GetLedState(kLedErr) == LedState::ON) {
      native_->EnablePORTB(1 << native::PB6);
    }
  } else if (row == 1) {
    native_->DisablePORTB(1 << native::PB6);
    if (GetLedState(kLedStatus) == LedState::ON) {
      native_->EnablePORTC(1 << native::PC6);
    }
  } else {
    native_->DisablePORTC(1 << native::PC6);
  }

  // Clear all row and column pins first. The column pins in PORTF should be
  // considered as active low (they need to be grounded to enable the LED).
  native_->DisablePORTB(0b00110000);
  native_->DisablePORTD(0b11010000);
  native_->EnablePORTF(0b00110011);

  // Enable the corect row pin and column pins for the current row.
  if (row == 0) {
    // row 0: 4 MSB of bank 0
    native_->EnablePORTD(1 << native::PD7);
    WriteColumns(kBank0);
  } else if (row == 1) {
    // row 1: 4 LSB of bank 0
    native_->EnablePORTB(1 << native::PB4);
    WriteColumns(kBank0 + 8);
  } else if (row == 2) {
    // row 2: 4 MSB of bank 1
    native_->EnablePORTD(1 << native::PD6);
    WriteColumns(kBank1);
  } else if (row == 3) {
    // row 3: 4 LSB of bank 1
    native_->EnablePORTD(1 << native::PD4);
    WriteColumns(kBank1 + 8);
  } else if (row == 4) {
    // row 4: R,G,B and PROG
    native_->EnablePORTB(1 << native::PB5);
    WriteColumns(kLedR);
  }
}

void LedController::SetBankValue(uint8_t bank, uint8_t val) {
  for (int8_t i = 7; i >= 0; i--) {
    SetLedState(bank + (i * 2), (LedState)(val & 1));
    val >>= 1;
  }
}

void LedController::SetLedState(int led_index, LedState state) {
  state_.SetBits(led_index, 2, (uint8_t)state);
}

LedController::LedState LedController::GetLedState(int led_index) {
  // Calculate the index of the LSB of the LedState in the containing byte.
  uint8_t bit_index = 6 - (led_index % 8);
  return (LedState)((state_.GetContainerByte(led_index) & (3 << bit_index)) >>
                    bit_index);
}

void LedController::WriteColumns(uint8_t col0) {
  if (GetLedState(col0) == LedState::ON) {
    native_->DisablePORTF(1 << native::PF0);
  }
  if (GetLedState(col0 + 2) == LedState::ON) {
    native_->DisablePORTF(1 << native::PF1);
  }
  if (GetLedState(col0 + 4) == LedState::ON) {
    native_->DisablePORTF(1 << native::PF4);
  }
  if (GetLedState(col0 + 6) == LedState::ON) {
    native_->DisablePORTF(1 << native::PF5);
  }
}

} // namespace threeboard
