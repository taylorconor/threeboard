#pragma once

#include <cstdint>
#include <vector>

#include "simulator/simavr/simavr.h"

namespace threeboard {
namespace simulator {

class I2cEeprom {
public:
  I2cEeprom(Simavr *simavr, uint32_t size_bytes, uint8_t address);

private:
  Simavr *simavr_;
  uint32_t size_bytes_;
  uint8_t address_;
  std::vector<uint8_t> buffer_;
};
} // namespace simulator
} // namespace threeboard
