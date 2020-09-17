#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>

#include "simulator/firmware.h"
#include "simulator/simulator_delegate.h"
#include "simulator/ui/ui.h"
#include "simulator/usb/host.h"

namespace threeboard {
namespace simulator {
class Simulator : public SimulatorDelegate {
public:
  Simulator();

  void Run();

private:
  void PrepareRenderState() final;
  void HandlePhysicalKeypress(char key, bool state) final;
  void HandleVirtualKeypress(uint8_t mod_code, uint8_t key_code) final;
  uint16_t GetGdbPort() final;

  std::unique_ptr<UI> ui_;
  std::unique_ptr<Firmware> firmware_;
  std::unique_ptr<Host> usb_host_;
  std::mutex mutex_;
  std::condition_variable sim_run_var_;
  bool gdb_enabled_;
};
} // namespace simulator
} // namespace threeboard
