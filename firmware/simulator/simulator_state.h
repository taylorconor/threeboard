#pragma once

#include <ostream>
#include <string>

namespace threeboard {
namespace simulator {

struct DeviceState {
  uint8_t bank_0 = 0;
  uint8_t bank_1 = 0;
  bool led_r = false;
  bool led_g = false;
  bool led_b = false;
  bool led_prog = false;
  bool led_err = false;
  bool led_status = false;
  std::string usb_buffer = "";

  bool operator==(const DeviceState& rhs) const {
    return (bank_0 == rhs.bank_0) && (bank_1 == rhs.bank_1) &&
           (led_r == rhs.led_r) && (led_g == rhs.led_g) &&
           (led_b == rhs.led_b) && (led_prog == rhs.led_prog) &&
           // Explicitly disable ERR and STATUS LEDs from comparison for now.
           /*(led_err == rhs.led_err) && (led_status == rhs.led_status) &&*/
           (usb_buffer == rhs.usb_buffer);
  }

  bool operator!=(const DeviceState& rhs) const { return !operator==(rhs); }

  friend std::ostream& operator<<(std::ostream& os, const DeviceState& state) {
    os << "(";
    os << "B0=" << (int)state.bank_0 << ",";
    os << "B1=" << (int)state.bank_1 << ",";
    os << "R=" << state.led_r << ",";
    os << "G=" << state.led_g << ",";
    os << "B=" << state.led_b << ",";
    os << "PROG=" << state.led_prog << ",";
    os << "ERR=" << state.led_err << ",";
    os << "STATUS=" << state.led_status << ",";
    os << "USB='" << state.usb_buffer << "'";
    os << ")";
    return os;
  }
};

struct SimulatorState {
  int cpu_state = 0;
  bool gdb_enabled = false;
  bool usb_attached = false;
  uint16_t sram_usage = 0;
  uint16_t data_section_size = 0;
  uint16_t bss_section_size = 0;
  uint16_t stack_size = 0;
};
}  // namespace simulator
}  // namespace threeboard