#pragma once

#include "src/native/native.h"
#include "src/storage/internal/eeprom.h"

namespace threeboard {
namespace storage {

class InternalEeprom final : public Eeprom {
 public:
  explicit InternalEeprom(native::Native *native);

  bool ReadByte(const uint16_t &byte_offset, uint8_t *data) override;
  bool WriteByte(const uint16_t &byte_offset, uint8_t data) override;

 private:
  native::Native *native_;
};

}  // namespace storage
}  // namespace threeboard