#pragma once

#include "simulator.h"

#include <condition_variable>
#include <memory>
#include <mutex>

#include "simulator/simulator_delegate.h"
#include "simulator/ui/ui.h"

namespace threeboard {
namespace simulator {
class Runner : public SimulatorDelegate {
public:
  Runner();

  void RunSimulator();

private:
  void PrepareRenderState() final;
  void HandleKeypress(char key, bool state) final;
  uint16_t GetGdbPort() final;

  std::unique_ptr<UI> ui_;
  std::unique_ptr<Simulator> simulator_;
  std::mutex mutex_;
  std::condition_variable cond_var_;
  bool gdb_enabled_;
};
} // namespace simulator
} // namespace threeboard
