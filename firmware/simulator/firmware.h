#pragma once

#include <atomic>
#include <functional>
#include <thread>

#include "simavr/sim_avr.h"
#include "simulator/core/sim_32u4.h"
#include "simulator/firmware_state_delegate.h"

namespace threeboard {
namespace simulator {

class Firmware : public FirmwareStateDelegate {
public:
  Firmware();
  ~Firmware();

  void RunAsync();
  void Reset();

  avr_t *GetAvr() const;

  // Retrieve ports containing output pins.
  uint8_t GetPortB() const;
  uint8_t GetPortC() const;
  uint8_t GetPortD() const;
  uint8_t GetPortF() const;

  // Set ports containing input pins.
  void SetPinB(uint8_t, bool);

  int GetCpuState() const final;
  uint64_t GetCpuCycleCount() const final;
  bool IsGdbEnabled() const final;

  void EnableGdb(uint16_t port) const;
  void DisableGdb() const;

private:
  void RunDetached();

  std::atomic<bool> is_running_;
  std::atomic<bool> should_reset_;

  std::unique_ptr<avr_t> avr_;
  mcu_t *mcu_;
  std::thread sim_thread_;
};
} // namespace simulator
} // namespace threeboard
