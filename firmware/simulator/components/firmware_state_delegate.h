#pragma once

#include <cstdint>

namespace threeboard {
namespace simulator {

class FirmwareStateDelegate {
public:
  virtual int GetCpuState() const = 0;
  virtual uint64_t GetCpuCycleCount() const = 0;
  virtual bool IsGdbEnabled() const = 0;

protected:
  virtual ~FirmwareStateDelegate() = default;
};
} // namespace simulator
} // namespace threeboard
