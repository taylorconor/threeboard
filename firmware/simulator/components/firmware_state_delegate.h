#pragma once

#include <cstdint>

namespace threeboard {
namespace simulator {

// A delegate interface for reading state about the firmware being simulated.
class FirmwareStateDelegate {
 public:
  virtual int GetCpuState() const = 0;
  virtual uint64_t GetCpuCycleCount() const = 0;
  virtual bool IsGdbEnabled() const = 0;

  virtual uint16_t GetDataSectionSize() const = 0;
  virtual uint16_t GetBssSectionSize() const = 0;
  virtual uint16_t GetStackSize() const = 0;
  virtual uint16_t GetSramUsage() const = 0;

 protected:
  virtual ~FirmwareStateDelegate() = default;
};
}  // namespace simulator
}  // namespace threeboard
