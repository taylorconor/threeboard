#include "internal_eeprom.h"

#include "src/util/util.h"

namespace threeboard {
namespace storage {

InternalEeprom::InternalEeprom(native::Native *native) : native_(native) {}

bool InternalEeprom::Read(const uint16_t &byte_offset, uint8_t *data,
                          const uint16_t &length) {
  native_->EepromRead(byte_offset, data, length);
  return true;
}

bool InternalEeprom::Write(const uint16_t &byte_offset, uint8_t *data,
                           const uint16_t &length) {
  native_->EepromWrite(byte_offset, data, length);
  return true;
}

}  // namespace storage
}  // namespace threeboard
