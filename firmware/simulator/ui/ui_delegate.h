#pragma once

namespace threeboard {
namespace simulator {

enum class SimulatorSource {
  SIMULATOR,
  SIMAVR,
};

class UIDelegate {
 public:
  virtual void HandleLogLine(const std::string& log_line) = 0;
  virtual void HandleLogLine(const std::string& log_line,
                             const SimulatorSource& source) = 0;
};

}  // namespace simulator
}  // namespace threeboard