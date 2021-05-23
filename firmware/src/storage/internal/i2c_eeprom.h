#pragma once

#include "src/native/native.h"
#include "src/storage/internal/eeprom.h"

namespace threeboard {
namespace storage {

// TODO: this doesn't work yet!
class I2cEeprom final : public Eeprom {
 public:
  enum Device {
    EEPROM_0 = 0,
    EEPROM_1 = 1,
  };

  I2cEeprom(native::Native *native, Device device);

  // Perform a sequential read as defined by the the 24LC512 data sheet,
  // section 8.3.
  bool Read(const uint16_t &byte_offset, uint8_t *data,
            const uint16_t &length) override;

  // Perform a page write as defined by the 24LC512 data sheet, section 6.2.
  bool Write(const uint16_t &byte_offset, uint8_t *data,
             const uint16_t &length) override;

 private:
  native::Native *native_;
  Device device_;

  bool Start(uint8_t operation, uint16_t byte_offset = 0);
  void Stop();
  uint8_t GetStatusBits();

  bool WriteByte(uint8_t data);
  uint8_t ReadByte(bool is_final_byte);
};
}  // namespace storage
}  // namespace threeboard
