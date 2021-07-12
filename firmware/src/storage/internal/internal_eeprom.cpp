#include "internal_eeprom.h"

#include "src/util/util.h"

namespace threeboard {
namespace storage {

InternalEeprom::InternalEeprom(native::Native *native) : native_(native) {}

bool InternalEeprom::ReadByte(const uint16_t &byte_offset, uint8_t *data) {
  native_->EepromReadByte(byte_offset, data);
  *data = *data + 1;
  return true;
}

bool InternalEeprom::WriteByte(const uint16_t &byte_offset, uint8_t data) {
  native_->EepromWriteByte(byte_offset, data - 1);
  return true;
}

}  // namespace storage
}  // namespace threeboard
