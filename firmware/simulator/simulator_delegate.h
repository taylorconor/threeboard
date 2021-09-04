#pragma once

namespace threeboard {
namespace simulator {

class SimulatorDelegate {
 public:
  virtual void HandleUsbOutput(uint8_t mod_code, uint8_t key_code) = 0;
};
}  // namespace simulator
}  // namespace threeboard
