#pragma once

#include <atomic>
#include <functional>
#include <thread>

#include "simulator/components/firmware_state_delegate.h"
#include "simulator/simavr/simavr.h"

namespace threeboard {
namespace simulator {

class Firmware : public FirmwareStateDelegate {
public:
  explicit Firmware(Simavr *simavr);
  ~Firmware() override;

  void RunAsync();
  void Reset();

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

  uint16_t GetDataSectionSize() const final;
  uint16_t GetBssSectionSize() const final;
  uint16_t GetStackSize() const final;
  uint16_t GetSramUsage() const final;

  void EnableGdb(uint16_t port) const;
  void DisableGdb() const;

private:
  void RunDetached();

  Simavr *simavr_;

  // This thread is used to offload the simavr run loop into its own thread.
  std::thread sim_thread_;

  // True if simavr is running. This can be updated from multiple threads.
  std::atomic<bool> is_running_;

  // True if we have been told to reset simavr on its next clock cycle.
  std::atomic<bool> should_reset_;
};
} // namespace simulator
} // namespace threeboard
