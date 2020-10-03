#pragma once

#include <stdint.h>

namespace threeboard {
namespace native {

class I2C {
public:
  void Init();

  // TODO: return status?
  void Write(const uint16_t &address, uint8_t *data, const uint16_t &length);
  void SequentialRead(const uint16_t &address, uint8_t *data,
                      const uint16_t &length);
};
} // namespace native
} // namespace threeboard
