#include "i2c_eeprom.h"

#include "src/logging.h"
#include "src/util/util.h"

namespace threeboard {
namespace storage {
namespace {

constexpr uint8_t kWriteBit = 0;
constexpr uint8_t kReadBit = 1;

uint8_t CreateControlByte(I2cEeprom::Device device, uint8_t operation) {
  return 0b10100000 | ((device & 7) << 1) | (operation & 1);
}

}  // namespace

I2cEeprom::I2cEeprom(native::Native *native, Device device)
    : native_(native), device_(device) {}

bool I2cEeprom::Read(const uint16_t &byte_offset, uint8_t *data,
                     const uint16_t &length) {
  RETURN_IF_ERROR(StartAndAddress(kReadBit, byte_offset), Stop());
  uint16_t i = 0;
  for (; i < length - 1; i++) {
    *(data + i) = ReadByte(false);
  }
  *(data + i) = ReadByte(true);
  Stop();

  return true;
}

bool I2cEeprom::Write(const uint16_t &byte_offset, uint8_t *data,
                      const uint16_t &length) {
  LOG("I2cEeprom::Write");
  // TODO: Implement page-write optimisation.
  for (uint16_t i = 0; i < length; i++) {
    RETURN_IF_ERROR(StartAndAddress(kWriteBit, byte_offset + i));
    RETURN_IF_ERROR(WriteByteAndAck(*(data + i)));
    Stop();
  }
  return true;
}

bool I2cEeprom::Start(uint8_t operation) {
  // Send START and wait for it to complete.
  native_->SetTWCR((1 << native::TWINT) | (1 << native::TWSTA) |
                   (1 << native::TWEN));
  while (!(native_->GetTWCR() & (1 << native::TWINT))) {
    LOG("I2cEeprom::Start: looping TWINT");
  }

  // Verify that the start condition is acknowledged. For repeated start
  // (REP_START) the ack from the EEPROM is the same, but the AVR TWI module
  // generates a different status code to indicate ack-ing of a repeated start,
  // so we need to check both here.
  if (GetStatusBits() != native::TW_START &&
      GetStatusBits() != native::TW_REP_START) {
    return false;
  }

  // The control byte consists of: A 4 bit control code, 3 bit chip select code
  // (determined by the EEPROM's wiring), and 1 bit read=1/write=0. This will
  // clear the previously set TWSTA, so we don't need to explicitly clear it
  // here.
  LOG("I2cEeprom::Start: writing control byte for operation: %s",
      operation == kReadBit ? "READ" : "WRITE");
  RETURN_IF_ERROR(WriteByteAndAck(CreateControlByte(device_, operation)));
  return true;
}

bool I2cEeprom::StartAndAddress(uint8_t operation, uint16_t byte_offset) {
  // Execute the EEPROM device addressing sequence. See the 24LC512 data sheet,
  // section 5.0 for full details. In summary, the control and addressing
  // sequence involves sending three bytes to the slave device:
  // |-- control byte --| |-- address high byte --| |-- address low byte --|
  //
  // The address word must always be sent as a write operation. This means that
  // for random reads, two control bytes must be sent; the first triggers a
  // write operation before the two address bytes are sent, then the second
  // control byte is sent to start the read operation.
  RETURN_IF_ERROR(Start(kWriteBit));

  // The next two bytes are the byte offset (address) of the first data byte
  // to be written/read in this operation. The high byte is sent first.
  RETURN_IF_ERROR(WriteByteAndAck(byte_offset >> 8));
  RETURN_IF_ERROR(WriteByteAndAck(byte_offset));

  // If we're starting a read operation, we need to send another control byte to
  // set the device to read mode. For write operations this is not needed, as
  // the address low byte can immediately be followed by data to write.
  if (operation == kReadBit) {
    RETURN_IF_ERROR(Start(kReadBit));
  }
  return true;
}

void I2cEeprom::Stop() {
  // Send STOP and wait for it to complete.
  native_->SetTWCR((1 << native::TWINT) | (1 << native::TWEN) |
                   (1 << native::TWSTO));
  while (native_->GetTWCR() & (1 << native::TWSTO))
    ;
}

uint8_t I2cEeprom::GetStatusBits() {
  // Mask out non-status bits of the TWI status register.
  return native_->GetTWSR() & 0xF8;
}

bool I2cEeprom::WriteByteAndAck(uint8_t data) {
  WriteByte(data);
  auto status_bits = GetStatusBits();
  // TODO: This fails when attempting to SLA+R: We expect TW_MR_SLA_ACK, we
  // receive TW_REP_START (i assume because the status hasn't been cleared yet).
  // It succeeds for SLA+W.
  if (status_bits != native::TW_MT_SLA_ACK &&
      status_bits != native::TW_MR_SLA_ACK) {
    LOG("I2cEeprom::WriteByteAndAck: fail status: %d", status_bits);
    return false;
  }
  return true;
}

void I2cEeprom::WriteByte(uint8_t data) {
  native_->SetTWDR(data);
  native_->SetTWCR((1 << native::TWINT) | (1 << native::TWEN));
  while (!(native_->GetTWCR() & (1 << native::TWINT))) {
    LOG("I2cEeprom::WriteByte: looping TWINT");
  }
}

uint8_t I2cEeprom::ReadByte(bool is_final_byte) {
  uint8_t twcr = (1 << native::TWINT) | (1 << native::TWEN);
  if (!is_final_byte) {
    twcr |= (1 << native::TWEA);
  }
  native_->SetTWCR(twcr);
  while (!(native_->GetTWCR() & (1 << native::TWINT))) {
    LOG("I2cEeprom::ReadByte: looping TWINT");
  }
  return native_->GetTWDR();
}
}  // namespace storage
}  // namespace threeboard
