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
    PULSE = 3,
  };

  // Full packed LED state, including individual pulse timers for each LED
  // occupying the remaining bits in a uint8_t after storing the State.
  struct FullState {
    State state : 2;
    uint8_t pulse_timer : 6;

    FullState(State state) : state(state), pulse_timer(0) {}
  };

  // Getters and setters for all available addressable LEDs.
  void SetBank0(uint8_t val) { bank_0_ = val; }
  uint8_t GetBank0() const { return bank_0_; }
  void SetBank1(uint8_t val) { bank_1_ = val; }
  uint8_t GetBank1() const { return bank_1_; }
  void SetR(State state) { led_r_ = state; }
  FullState* GetR() { return &led_r_; }
  void SetG(State state) { led_g_ = state; }
  FullState* GetG() { return &led_g_; }
  void SetB(State state) { led_b_ = state; }
  FullState* GetB() { return &led_b_; }
  void SetProg(State state) { led_prog_ = state; }
  FullState* GetProg() { return &led_prog_; }
  void SetErr(State state) { led_err_ = state; }
  FullState* GetErr() { return &led_err_; }
  void SetStatus(State state) { led_status_ = state; }
  FullState* GetStatus() { return &led_status_; }

 private:
  // The state of all of the LEDs in the threeboard. There are some wasted bits
  // here which can be compressed into a bitset/bitfield in the future if we're
  // tight on memory. But for now we have plenty.
  uint8_t bank_0_ = 0;
  uint8_t bank_1_ = 0;
  FullState led_r_ = OFF;
  FullState led_g_ = OFF;
  FullState led_b_ = OFF;
  FullState led_prog_ = OFF;
  FullState led_err_ = OFF;
  FullState led_status_ = OFF;
};
}  // namespace threeboard
