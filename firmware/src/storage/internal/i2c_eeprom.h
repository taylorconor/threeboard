#pragma once

#include "src/native/native.h"
#include "src/storage/internal/eeprom.h"

namespace threeboard {
namespace storage {

class I2cEeprom final : public Eeprom {
 public:
  enum Device {
    EEPROM_0 = 0,
    EEPROM_1 = 1,
  };

  I2cEeprom(native::Native *native, Device device);

  // Perform either a random or sequential read as defined by the the 24LC512
  // data sheet, section 8.3.
  bool ReadByte(const uint16_t &byte_offset, uint8_t *data) override;

  // Perform a page write as defined by the 24LC512 data sheet, section 6.2.
  bool WriteByte(const uint16_t &byte_offset, uint8_t data) override;

 private:
  native::Native *native_;
  Device device_;

  // The 24LC512 stores the last address read from or written to. When
  // performing sequential reads we can avoid sending the address word by
  // checking this address first.
  uint16_t prev_address_;

  bool Start(uint8_t operation);
  bool StartAndAddress(uint8_t operation, uint16_t byte_offset);
  void Stop();
  uint8_t GetStatusBits();

  bool WriteByteAndAck(uint8_t data);
  void WriteByte(uint8_t data);
  uint8_t ReadByte(bool is_final_byte);
};
}  // namespace storage
}  // namespace threeboard
