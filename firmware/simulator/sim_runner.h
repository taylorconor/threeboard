#pragma once

#include "sim_ui.h"
#include "simulator.h"

namespace threeboard {
namespace simulator {
class SimRunner {
public:
  SimRunner();

  void RunSimulator();

private:
  void UpdateSimState();
  void HandleKeypress(SimUI::Key key, bool state);

  std::unique_ptr<SimUI> sim_ui_;
  std::unique_ptr<Simulator> simulator_;
  std::mutex mutex_;
  std::condition_variable cond_var_;
};
} // namespace simulator
} // namespace threeboard
