#pragma once

#include <atomic>
#include <functional>
#include <thread>

#include "simulator/components/i2c_eeprom.h"
#include "simulator/components/uart.h"
#include "simulator/components/usb_host.h"
#include "simulator/simavr/simavr.h"
#include "simulator/ui/ui_delegate.h"
#include "simulator/util/state_storage.h"

namespace threeboard {
namespace simulator {

struct DeviceState {
  uint8_t bank_0 = 0;
  uint8_t bank_1 = 0;
  bool led_r = false;
  bool led_g = false;
  bool led_b = false;
  bool led_prog = false;
  bool led_err = false;
  bool led_status = false;
  std::string usb_buffer = "";
};

struct SimulatorState {
  DeviceState device_state;
  int cpu_state = 0;
  bool gdb_enabled = false;
  bool usb_attached = false;
  uint16_t sram_usage = 0;
  uint16_t data_section_size = 0;
  uint16_t bss_section_size = 0;
  uint16_t stack_size = 0;
};

class Simulator : public SimulatorDelegate {
 public:
  Simulator(Simavr *simavr, StateStorage *state_storage);
  ~Simulator();

  void RunAsync();
  void Reset();

  SimulatorState GetStateAndFlush();
  void HandleKeypress(char key, bool state);

  uint64_t GetCurrentCpuCycle();
  void ToggleGdb(uint16_t port) const;
  void EnableLogging(UIDelegate *ui_delegate);

 private:
  void HandleUsbOutput(uint8_t mod_code, uint8_t key_code) override;

  void InternalRunAsync();
  void UpdateDeviceState();

  Simavr *simavr_;
  std::unique_ptr<Uart> uart_;
  std::thread sim_thread_;
  std::atomic<bool> is_running_;
  std::atomic<bool> should_reset_;
  DeviceState device_state_;
  UsbHost usb_host_;
  I2cEeprom eeprom0_;
};

}  // namespace simulator
}  // namespace threeboard