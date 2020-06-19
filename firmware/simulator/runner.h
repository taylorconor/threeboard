#pragma once

#include "simulator.h"

#include <condition_variable>
#include <memory>
#include <mutex>

#include "simulator/simulator_delegate.h"
#include "simulator/ui/key.h"
#include "simulator/ui/ui.h"

namespace threeboard {
namespace simulator {
class Runner : public SimulatorDelegate {
public:
  Runner();

  void RunSimulator();

private:
  //  void UpdateSimState();
  //  void HandleKeypress(UI::Key key, bool state);

  void PrepareRenderState() final;
  void HandleKeypress(Key key, bool state) final;
  void EnableGdb() final;
  void DisableGdb() final;
  uint16_t GetGdbPort() final;

  std::unique_ptr<UI> ui_;
  std::unique_ptr<Simulator> simulator_;
  std::mutex mutex_;
  std::condition_variable cond_var_;
};
} // namespace simulator
} // namespace threeboard
