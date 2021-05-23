#pragma once

#include "src/native/native.h"
#include "src/storage/internal/eeprom.h"

namespace threeboard {
namespace storage {

class InternalEeprom final : public Eeprom {
 public:
  explicit InternalEeprom(native::Native *native);

  bool Read(const uint16_t &byte_offset, uint8_t *data,
            const uint16_t &length) override;
  bool Write(const uint16_t &byte_offset, uint8_t *data,
             const uint16_t &length) override;

 private:
  native::Native *native_;
};

}  // namespace storage
}  // namespace threeboard