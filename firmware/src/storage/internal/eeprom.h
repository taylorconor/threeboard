#pragma once

#include <stdint.h>

namespace threeboard {
namespace storage {

class Eeprom {
 public:
  virtual ~Eeprom() = default;

  virtual bool Read(const uint16_t &byte_offset, uint8_t *data,
                    const uint16_t &length) = 0;
  virtual bool Write(const uint16_t &byte_offset, uint8_t *data,
                     const uint16_t &length) = 0;
};

}  // namespace storage
}  // namespace threeboard