#include "led_controller.h"

namespace threeboard {
namespace {

// defines indexes into the `state_` bitset.
#define BANK_0 0
#define BANK_1 16
#define LED_R 32
#define LED_G 34
#define LED_B 36
#define LED_PROG 38
#define LED_ERR 40
#define LED_STATUS 42
#define BLINK_STATUS 44
#define PREV_SCAN_LINE 45

/**
   Pin mappings:
   STATUS (status1) - 36 - PF7
   ERR (status0) - 37 - PF6
   row0 - 38 - PF5
   row1 - 39 - PF4
   row2 - 40 - PF1
   row3 - 41 - PF0
   row4 - 42 - PB5
   col0 - 25 - PD4
   col1 - 26 - PD6
   col2 - 27 - PD7
   col3 - 28 - PB4
 */

} // namespace

LedController::LedController(native::Native *native) : native_(native) {
  // Specify which pins will be used by this controller.
  native_->EnableDDRB(0b00110000);
  native_->EnableDDRD(0b11010000);
  native_->EnableDDRF(0b11110011);
}

void LedController::ScanNextLine() {
  // The scan line occupies the last 3 bits in the bitset. It should be
  // overwritten on each scan.
  uint8_t scan_line = state_.GetContainerByte(PREV_SCAN_LINE) & 7;
  WriteStateToPins(scan_line);
  state_.SetBits(PREV_SCAN_LINE, 3, (scan_line + 1) % 5);
}

void LedController::SetBank0(uint8_t val) { SetBankValue(BANK_0, val); }
void LedController::SetBank1(uint8_t val) { SetBankValue(BANK_1, val); }
void LedController::SetR(LedState state) { SetLedState(LED_R, state); }
void LedController::SetG(LedState state) { SetLedState(LED_G, state); }
void LedController::SetB(LedState state) { SetLedState(LED_B, state); }
void LedController::SetProg(LedState state) { SetLedState(LED_PROG, state); }
void LedController::SetErr(LedState state) { SetLedState(LED_ERR, state); }
void LedController::SetStatus(LedState state) {
  SetLedState(LED_STATUS, state);
}

void LedController::WriteStateToPins(uint8_t row) {
  // ERR and STATUS are a special case since they're mutally exclusive LEDs.
  // They could be set on each scan, but to maintain consistent brightness they
  // have each been assigned their own scan line.
  /*if (row == 0) {
    if (GetLedState(LED_ERR) == LedState::ON) {
      native_->EnablePORTF(1 << native::PF6);
    }
  } else if (row == 1) {
    native_->DisablePORTF(1 << native::PF6);
    if (GetLedState(LED_STATUS) == LedState::ON) {
      native_->EnablePORTF(1 << native::PF7);
    }
  } else {
    native_->DisablePORTF(1 << native::PF7);
    }*/

  // Clear all row and column pins first.
  native_->DisablePORTB(0b00100000);
  native_->EnablePORTB(0b00010000);
  native_->DisablePORTF(0b00110011);
  native_->EnablePORTD(0b11010000);

  // Enable the corect row pin and column pins for the current row.
  if (row == 0) {
    // row 0: 4 MSB of bank 0
    native_->EnablePORTF(1 << native::PF5);
    WriteColumns(BANK_0);
  } else if (row == 1) {
    // row 1: 4 LSB of bank 0
    native_->EnablePORTF(1 << native::PF4);
    WriteColumns(BANK_0 + 8);
  } else if (row == 2) {
    // row 2: 4 MSB of bank 1
    native_->EnablePORTF(1 << native::PF1);
    WriteColumns(BANK_1);
  } else if (row == 3) {
    // row 3: 4 LSB of bank 1
    native_->EnablePORTF(1 << native::PF0);
    WriteColumns(BANK_1 + 8);
  } else if (row == 4) {
    // row 4: R,G,B and PROG
    native_->EnablePORTB(1 << native::PB5);
    WriteColumns(LED_R);
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
    native_->DisablePORTD(1 << native::PD4);
  }
  if (GetLedState(col0 + 2) == LedState::ON) {
    native_->DisablePORTD(1 << native::PD6);
  }
  if (GetLedState(col0 + 4) == LedState::ON) {
    native_->DisablePORTD(1 << native::PD7);
  }
  if (GetLedState(col0 + 6) == LedState::ON) {
    native_->DisablePORTB(1 << native::PB4);
  }
}

} // namespace threeboard
