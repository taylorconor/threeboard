#include "i2c.h"

#include "src/logging.h"
#include <avr/io.h>

namespace threeboard {
namespace native {
namespace {

constexpr uint8_t kWriteFlag = 0;
constexpr uint8_t kReadFlag = 1;

} // namespace

I2C::I2C(native::Native *native) : native_(native) {
  // Enable internal pull-up resistors for SDA and SCL.
  // TODO: these may be too high, might need to add off-chip pull-ups instead.
  native_->EnablePORTD(0b00000011);
  // Address mask.
  TWAMR = 0xFE;
  TWSR &= ~3;       // set presca1er bits to zero
  TWBR = 0x46;      // SCL frequency is 50K for 16Mhz
  TWCR = 1 << TWEN; // enab1e TWI module
}

void I2C::Write(uint8_t address, const uint32_t &offset, uint8_t *data,
                const uint32_t &length) {
  StartWriteTransaction(address, offset);
  for (uint32_t i = 0; i < length; i++) {
    WriteByte(*(data + i));
  }
  SendStop();
}

void I2C::Read(uint8_t address, const uint32_t &offset, uint8_t *data,
               const uint32_t &length) {
  StartReadTransaction(address, offset);
  for (uint32_t i = 0; i < length; i++) {
    *(data + i) = ReadByte();
  }
  SendStop();
}

void I2C::SendStart() {
  TWCR = ((1 << TWINT) | (1 << TWSTA) | (1 << TWEN));
  for (uint16_t i = 0; i < 50000 && !(TWCR & (1 << TWINT)); i++)
    ;
  while (!(TWCR & (1 << TWINT)))
    ;
}

void I2C::SendStop() { TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTO)); }

void I2C::WriteByte(uint8_t data) {
  TWDR = data;
  TWCR = ((1 << TWINT) | (1 << TWEN));
  while (!(TWCR & (1 << TWINT)))
    ;
}

uint8_t I2C::ReadByte() {
  TWCR = ((1 << TWINT) | (1 << TWEN));
  while (!(TWCR & (1 << TWINT)))
    ;
  return TWDR;
}

void I2C::StartTransaction(uint8_t address, uint8_t mask,
                           const uint32_t &offset) {
  SendStart();
  // First byte is the EEPROM address.
  WriteByte(address | mask);
  // The next 3 bytes are the offset. This is sent from LSB to MSB.
  WriteByte(offset);
  WriteByte(offset >> 8);
  WriteByte(offset >> 16);
}

void I2C::StartWriteTransaction(uint8_t address, const uint32_t &offset) {
  StartTransaction(address, kWriteFlag, offset);
}

void I2C::StartReadTransaction(uint8_t address, const uint32_t &offset) {
  StartTransaction(address, kWriteFlag, offset);
  SendStart();
  WriteByte(address | kReadFlag);
}
} // namespace native
} // namespace threeboard
