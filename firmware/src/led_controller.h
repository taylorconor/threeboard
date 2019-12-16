#pragma once

#include "native/native.h"
#include "util/bitset.h"
#include <stdint.h>

namespace threeboard {

// A class to abstract away the LED interaction, scanning and timing into an
// easy to use interface. It relies on an external clock pulse in the form of
// calls to `ScanNextLine` which is provided by an interrupt handler polled by
// timer1 every 2ms.
class LedController {
public:
  // State of individual addressable LEDs.
  enum class State {
    OFF = 0,
    ON = 1,
    BLINK = 2,
    BLINK_FAST = 3,
  };

  LedController(native::Native *native);

  // Handles rendering of the next scan row. Called by the timer interrupt
  // handler every 2ms.
  void ScanNextLine();

  // Setters for all available addressable LEDs.
  void SetBank0(uint8_t val);
  void SetBank1(uint8_t val);
  void SetR(State state);
  void SetG(State state);
  void SetB(State state);
  void SetProg(State state);
  void SetErr(State state);
  void SetStatus(State state);

private:
  // The state of the LEDs (bitset of `State` values).
  util::bitset<48> state_;
  native::Native *native_;

  // Set the numeric (i.e. non-blink) value of one of the LED banks.
  void SetBankValue(uint8_t bank, uint8_t val);
  // Set the state of an individual LED.
  void SetLedState(int led_index, State state);
  // Copy `state_` out to the hardware pins for a given row.
  void WriteStateToPins(uint8_t row);
  // Retrieve an LED `State` from the `state_` bitset.
  State GetLedState(int led_index);
  // Copy the state for the given column out to the hardware pins.
  void WriteColumns(uint8_t col0);
};
} // namespace threeboard
