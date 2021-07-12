#pragma once

#include <stdint.h>

namespace threeboard {
namespace storage {

class Eeprom {
 public:
  virtual ~Eeprom() = default;

  virtual bool ReadByte(const uint16_t &byte_offset, uint8_t *data) = 0;
  virtual bool WriteByte(const uint16_t &byte_offset, uint8_t data) = 0;
};

}  // namespace storage
}  // namespace threeboard