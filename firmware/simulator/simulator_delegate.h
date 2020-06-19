#pragma once

#include "simulator/ui/key.h"

namespace threeboard {
namespace simulator {

class SimulatorDelegate {
public:
  virtual void PrepareRenderState() = 0;
  virtual void HandleKeypress(Key key, bool state) = 0;
  virtual void EnableGdb() = 0;
  virtual void DisableGdb() = 0;
  virtual uint16_t GetGdbPort() = 0;
};
} // namespace simulator
} // namespace threeboard
