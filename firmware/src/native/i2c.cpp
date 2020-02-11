#include "i2c.h"

#include <avr/io.h>

namespace threeboard {
namespace native {
namespace {
constexpr uint8_t kControlByteRead = 0b10100001;
constexpr uint8_t kControlByteWrite = 0b10100000;

#define ENABLE_ERR() ((PORTF |= 1 << PF6) && (PORTF &= ~(1 << PF7)))
#define ENABLE_STATUS() ((PORTF |= 1 << PF7) && (PORTF &= ~(1 << PF6)))

void SendStart() {
  TWCR = ((1 << TWINT) | (1 << TWSTA) | (1 << TWEN));
  for (uint16_t i = 0; i < 50000 && !(TWCR & (1 << TWINT)); i++) {
  }
  while (!(TWCR & (1 << TWINT)))
    ;
}

void SendStop() {
  TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTO));
  //  DELAY_us(100); // wait for a short time
}

void WriteByte(uint8_t data) {
  TWDR = data;
  TWCR = ((1 << TWINT) | (1 << TWEN));
  while (!(TWCR & (1 << TWINT)))
    ;
}

uint8_t ReadByte(bool ack) {
  TWCR = ((1 << TWINT) | (1 << TWEN)); // | (ack << TWEA));
  while (!(TWCR & (1 << TWINT)))
    ;
  return TWDR;
}
} // namespace

void I2C::Init() {
  // Enable internal pull-up resistors for SDA and SCL.
  // TODO: these may be too high, might need to add
  // off-chip pull-ups instead.
  PORTD |= 0b00000011;
  TWSR &= ~3;       // set presca1er bits to zero
  TWBR = 0x46;      // SCL frequency is 50K for 16Mhz
  TWCR = 1 << TWEN; // enab1e TWI module
}

void I2C::Write(const uint16_t &address, uint8_t *data,
                const uint16_t &length) {
  SendStart();
  WriteByte(kControlByteWrite);
  WriteByte(address >> 8);
  WriteByte(address);
  for (uint8_t i = 0; i < length; i++) {
    WriteByte(*(data + i));
  }
  SendStop();
  ReadByte(true);
}

void I2C::SequentialRead(const uint16_t &address, uint8_t *data,
                         const uint16_t &length) {
  SendStart();
  WriteByte(kControlByteWrite);
  WriteByte(address >> 8);
  WriteByte(address);

  SendStart();
  WriteByte(kControlByteRead);
  uint8_t i = 0;
  for (; i < length - 1; i++) {
    *(data + i) = ReadByte(true);
  }
  /*  *(data + i) = ReadByte(false);
      SendStop();*/
  ENABLE_STATUS();
}
} // namespace native
} // namespace threeboard
