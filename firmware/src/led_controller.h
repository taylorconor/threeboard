#pragma once

#include <stdint.h>

#include "src/led_state.h"
#include "src/native/native.h"

namespace threeboard {

// A class to abstract away the LED interaction, scanning and timing. It relies
// on an external clock pulse in the form of calls to `ScanNextLine` which is
// provided by an interrupt handler polled by timer1 every 2ms.
class LedController {
 public:
  explicit LedController(native::Native *native);
  virtual ~LedController() = default;

  // Handles rendering of the next scan row. Called by the timer interrupt
  // handler every 2ms.
  virtual void ScanNextLine();

  // Handles timing of LED blinking. Called by the timer interrupt handler every
  // 5ms.
  virtual void UpdateBlinkStatus();

  // state_ is guaranteed to live for the entire lifetime of the firmware.
  virtual LedState *GetLedState() { return &state_; }

 protected:
  // A default constructor used by the LedControllerMock to avoid the
  // Native-dependent public constructor.
  LedController() = default;

 private:
  native::Native *native_;
  LedState state_;

  // The next LED line to scan.
  uint8_t next_scan_line_ = 0;

  // The status of LED blinking, and a timer used to control the blinking.
  uint8_t blink_status_ = 0;

  // Copy `state_` out to the hardware pins for a given row.
  void WriteStateToPins(uint8_t row);

  // Copy the provided state out to the LED column array for the LED banks.
  void WriteColumns(uint8_t vals);

  bool ShouldEnableBlinkingLed() const;

  void ApplyLedState(native::PortModFn port_mod_fn, uint8_t val,
                     LedState::State state);
};
}  // namespace threeboard
