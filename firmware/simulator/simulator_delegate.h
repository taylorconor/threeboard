#pragma once

#include <string>

#include "simulator/flags.h"

namespace threeboard {
namespace simulator {

class SimulatorDelegate {
 public:
  virtual void PrepareRenderState() = 0;
  virtual void HandlePhysicalKeypress(char key, bool state) = 0;
  virtual void HandleVirtualKeypress(uint8_t mod_code, uint8_t key_code) = 0;
  virtual void HandleUartLogLine(const std::string& log_line) = 0;
  virtual Flags* GetFlags() = 0;
  virtual bool IsUsbAttached() = 0;

 protected:
  virtual ~SimulatorDelegate() = default;
};
}  // namespace simulator
}  // namespace threeboard
