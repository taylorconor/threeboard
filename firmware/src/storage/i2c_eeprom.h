#pragma once

#include "src/native/native.h"

namespace threeboard {
namespace storage {

// TODO: this doesn't work yet!
class I2cEeprom {
 public:
  explicit I2cEeprom(native::Native *native);

  enum Device {
    EEPROM_0 = 0,
    EEPROM_1 = 1,
  };

  enum Operation {
    WRITE = 0,
    READ = 1,
  };

  // Perform a sequential read as defined by the the 24LC512 data sheet,
  // section 8.3.
  bool Read(Device device, const uint16_t &byte_offset, uint8_t *data,
            const uint16_t &length);

  // Perform a page write as defined by the 24LC512 data sheet, section 6.2.
  bool Write(Device device, const uint16_t &byte_offset, uint8_t *data,
             const uint16_t &length);

 private:
  native::Native *native_;

  bool Start(Device device, Operation operation, uint16_t byte_offset = 0);
  void Stop();
  uint8_t GetStatusBits();

  bool WriteByte(uint8_t data);
  uint8_t ReadByte(bool is_final_byte);
};
}  // namespace storage
}  // namespace threeboard
