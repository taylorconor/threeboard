#include "i2c.h"

#include "src/native/mcu.h"
#include "src/util/util.h"

namespace threeboard {
namespace native {
namespace {

uint8_t CreateControlByte(I2C::Device device, I2C::Operation operation) {
  return 0b10100000 | ((device & 7) << 1) | (operation & 1);
}

}  // namespace

I2C::I2C(native::Native *native) : native_(native) {
  // Set the TWI prescaler to 0.
  native_->SetTWSR(native_->GetTWSR() & ~3);
  // Set the SCL clock frequency for the TWI interface to 100kHz.
  native_->SetTWBR(((F_CPU / 100000) - 16) / 2);
}

bool I2C::Read(Device device, const uint16_t &byte_offset, uint8_t *data,
               const uint16_t &length) {
  // First write the byte offset to the specified device to update its internal
  // address pointer before we begin the sequential read.
  RETURN_IF_ERROR(Start(device, Operation::WRITE, byte_offset), Stop());
  RETURN_IF_ERROR(Start(device, Operation::READ), Stop());

  uint32_t i;
  for (i = 0; i < length - 1; i++) {
    *(data + i) = ReadByte(false);
  }
  *(data + i) = ReadByte(true);
  Stop();
  return true;
}

bool I2C::Write(Device device, const uint16_t &byte_offset, uint8_t *data,
                const uint16_t &length) {
  // A page write can contain up to 128 bytes in total. So we split the write
  // into 128-bit chunks to increase throughput.
  uint16_t i = 0;
  for (uint16_t page_id = 0; page_id < length / 128; ++page_id) {
    RETURN_IF_ERROR(
        Start(device, Operation::WRITE, byte_offset + (page_id * 128)));
    for (; i < length; i++) {
      RETURN_IF_ERROR(WriteByte(*(data + i)));
    }
    Stop();
  }
  return true;
}

bool I2C::Start(Device device, Operation operation, uint16_t byte_offset) {
  // Send START and wait for it to complete.
  native_->SetTWCR((1 << native::TWINT) | (1 << native::TWSTA) |
                   (1 << native::TWEN));
  while (!(native_->GetTWCR() & (1 << native::TWINT)))
    ;

  if (GetStatusBits() != native::TW_START &&
      GetStatusBits() != native::TW_REP_START) {
    return false;
  }

  // Now execute the EEPROM device addressing sequence. See the 24LC512 data
  // sheet, section 5.0.

  // The first byte the 24LC512 EEPROM should receive after the START condition
  // is a device-specific control byte.
  WriteByte(CreateControlByte(device, operation));

  // The 24LC512 outputs an ACK signal on the SDA line after the first byte of
  // the address sequence (the control byte). If this doesn't happen, something
  // is wrong and we should abort.
  if (GetStatusBits() != native::TW_MT_SLA_ACK &&
      GetStatusBits() != native::TW_MR_SLA_ACK) {
    return false;
  }

  if (operation == Operation::WRITE) {
    // The next two bytes are the byte offset (address) of the first data byte
    // to be written/read in this operation. This only happens for WRITE
    // operations because a READ will first issue a WRITE to set the address
    // before continuing with a READ. The high byte is written first, followed
    // by the low byte.
    WriteByte(byte_offset >> 8);
    WriteByte(byte_offset);
  }

  return true;
}

void I2C::Stop() {
  // Send STOP and wait for it to complete.
  native_->SetTWCR((1 << native::TWINT) | (1 << native::TWEN) |
                   (1 << native::TWSTO));
  while (native_->GetTWCR() & (1 << native::TWSTO))
    ;
}

uint8_t I2C::GetStatusBits() {
  // Mask out non-status bits of the TWI status register.
  return native_->GetTWSR() & 0xF8;
}

bool I2C::WriteByte(uint8_t data) {
  native_->SetTWDR(data);
  native_->SetTWCR((1 << native::TWINT) | (1 << native::TWEN));
  while (!(native_->GetTWCR() & (1 << native::TWINT)))
    ;
  if (GetStatusBits() != native::TW_MT_DATA_ACK) {
    return false;
  }
  return true;
}

uint8_t I2C::ReadByte(bool is_final_byte) {
  uint8_t twcr = (1 << native::TWINT) | (1 << native::TWEN);
  if (!is_final_byte) {
    twcr |= (1 << native::TWEA);
  }
  native_->SetTWCR(twcr);
  while (!(native_->GetTWCR() & (1 << native::TWINT)))
    ;
  return native_->GetTWDR();
}
}  // namespace native
}  // namespace threeboard
