#pragma once

namespace threeboard {
namespace simulator {

// This delegate enables propagation of usb codes from the threeboard firmware
// to the simulator.
class SimulatorDelegate {
 public:
  virtual void HandleUsbOutput(uint8_t mod_code, uint8_t key_code) = 0;
};
}  // namespace simulator
}  // namespace threeboard
