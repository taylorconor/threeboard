#pragma once

#include <stdint.h>

#include "src/native/native.h"

namespace threeboard {

// A class to abstract away the LED interaction, scanning and timing. It relies
// on an external clock pulse in the form of calls to `ScanNextLine` which is
// provided by an interrupt handler polled by timer1 every 2ms.
class LedController {
public:
  // State of individual addressable LEDs.
  // TODO: implement support for blinking LED states.
  enum class LedState {
    OFF = 0,
    ON = 1,
    BLINK = 2,
    BLINK_FAST = 3,
  };

  explicit LedController(native::Native *native);

  // Handles rendering of the next scan row. Called by the timer interrupt
  // handler every 2ms.
  void ScanNextLine();

  // Setters for all available addressable LEDs.
  void SetBank0(uint8_t val);
  void SetBank1(uint8_t val);
  void SetR(LedState state);
  void SetG(LedState state);
  void SetB(LedState state);
  void SetProg(LedState state);
  void SetErr(LedState state);
  void SetStatus(LedState state);

private:
  native::Native *native_;

  // The state of all of the LEDs under the control of LedController. There are
  // some wasted bits here which can be compressed into a bitset/bitfield in the
  // future if we're tight on memory. But for now we have plenty.
  uint8_t bank_0_ = 0;
  uint8_t bank_1_ = 0;
  LedState led_r_ = LedState::OFF;
  LedState led_g_ = LedState::OFF;
  LedState led_b_ = LedState::OFF;
  LedState led_prog_ = LedState::OFF;
  LedState led_err_ = LedState::OFF;
  LedState led_status_ = LedState::OFF;

  // The next LED line to scan.
  uint8_t next_scan_line_ = 0;

  // The status of LED blinking, and a timer used to control the blinking.
  uint8_t blink_status_ = 0;

  // Copy `state_` out to the hardware pins for a given row.
  void WriteStateToPins(uint8_t row);

  // Copy the provided state out to the LED column array for the LED banks.
  void WriteColumns(uint8_t vals);

  void ApplyLedState(native::PortModFn port_mod_fn, uint8_t val,
                     LedState state);
};
} // namespace threeboard
