#pragma once

#include <stdint.h>

namespace threeboard {

// This class holds the state of each LED in the threeboard.
class LedState {
 public:
  // State of individual addressable LEDs.
  enum State : uint8_t {
    OFF = 0,
    ON = 1,
    BLINK = 2,
    BLINK_FAST = 3,
  };

  // Getters and setters for all available addressable LEDs.
  void SetBank0(uint8_t val) { bank_0_ = val; }
  uint8_t GetBank0() const { return bank_0_; }
  void SetBank1(uint8_t val) { bank_1_ = val; }
  uint8_t GetBank1() const { return bank_1_; }
  void SetR(State state) { led_r_ = state; }
  State GetR() const { return led_r_; }
  void SetG(State state) { led_g_ = state; }
  State GetG() const { return led_g_; }
  void SetB(State state) { led_b_ = state; }
  State GetB() const { return led_b_; }
  void SetProg(State state) { led_prog_ = state; }
  State GetProg() const { return led_prog_; }
  void SetErr(State state) { led_err_ = state; }
  State GetErr() const { return led_err_; }
  void SetStatus(State state) { led_status_ = state; }
  State GetStatus() const { return led_status_; }

 private:
  // The state of all of the LEDs in the threeboard. There are some wasted bits
  // here which can be compressed into a bitset/bitfield in the future if we're
  // tight on memory. But for now we have plenty.
  uint8_t bank_0_ = 0;
  uint8_t bank_1_ = 0;
  State led_r_ = OFF;
  State led_g_ = OFF;
  State led_b_ = OFF;
  State led_prog_ = OFF;
  State led_err_ = OFF;
  State led_status_ = OFF;
};
}  // namespace threeboard
