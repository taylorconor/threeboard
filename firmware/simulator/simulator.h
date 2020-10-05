#pragma once

#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>

#include "simulator/components/firmware.h"
#include "simulator/components/uart.h"
#include "simulator/components/usb_host.h"
#include "simulator/simavr/simavr.h"
#include "simulator/simulator_delegate.h"
#include "simulator/ui/ui.h"

namespace threeboard {
namespace simulator {
class Simulator : public SimulatorDelegate {
public:
  explicit Simulator(Simavr *simavr);
  ~Simulator() override;

  void Run();

private:
  void PrepareRenderState() final;
  void HandlePhysicalKeypress(char key, bool state) final;
  void HandleVirtualKeypress(uint8_t mod_code, uint8_t key_code) final;
  void HandleUartLogLine(const std::string &log_line) final;
  uint16_t GetGdbPort() final;
  bool IsUsbAttached() final;

  Simavr *simavr_;
  std::atomic<bool> is_running_;
  std::unique_ptr<Firmware> firmware_;
  std::unique_ptr<UsbHost> usb_host_;
  std::unique_ptr<Uart> uart_;
  std::unique_ptr<UI> ui_;
  std::mutex mutex_;
  std::condition_variable sim_run_var_;
  std::string log_file_path_;
  std::ofstream log_stream_;
};
} // namespace simulator
} // namespace threeboard
