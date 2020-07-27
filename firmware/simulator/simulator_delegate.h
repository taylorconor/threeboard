#pragma once

namespace threeboard {
namespace simulator {

class SimulatorDelegate {
public:
  virtual void PrepareRenderState() = 0;
  virtual void HandleKeypress(char key, bool state) = 0;
  virtual uint16_t GetGdbPort() = 0;

protected:
  virtual ~SimulatorDelegate() {}
};
} // namespace simulator
} // namespace threeboard
