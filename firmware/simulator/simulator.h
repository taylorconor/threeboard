#pragma once

#include <atomic>
#include <functional>
#include <thread>

#include "simulator/components/i2c_eeprom.h"
#include "simulator/components/uart.h"
#include "simulator/components/usb_host.h"
#include "simulator/simavr/simavr.h"
#include "simulator/simulator_state.h"
#include "simulator/ui/ui_delegate.h"
#include "simulator/util/state_storage.h"

namespace threeboard {
namespace simulator {

class Simulator : public SimulatorDelegate {
 public:
  Simulator(Simavr *simavr, StateStorage *state_storage);
  virtual ~Simulator();

  void RunAsync();
  void Reset();

  DeviceState GetDeviceState();
  SimulatorState GetSimulatorState();
  void HandleKeypress(char key, bool state);

  uint64_t GetCurrentCpuCycle();
  void ToggleGdb(uint16_t port) const;
  void EnableLogging(UIDelegate *ui_delegate);

 private:
  void HandleUsbOutput(uint8_t mod_code, uint8_t key_code) override;
  void HandlePortWrite(uint8_t port, uint8_t value);
  void UpdateLedState(uint8_t row);

  void InternalRunAsync();

  Simavr *simavr_;
  std::unique_ptr<Uart> uart_;
  std::thread sim_thread_;
  std::atomic<bool> is_running_;
  std::atomic<bool> should_reset_;
  UsbHost usb_host_;
  I2cEeprom eeprom0_;
  DeviceState device_state_;

  std::unique_ptr<PortWriteCallback> portb_write_callback_;
  std::unique_ptr<PortWriteCallback> portd_write_callback_;
};

}  // namespace simulator
}  // namespace threeboard