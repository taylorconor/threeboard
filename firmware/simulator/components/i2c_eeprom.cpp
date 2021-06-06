#include "i2c_eeprom.h"

#include <functional>

#include "simulator/util/logging.h"

namespace threeboard {
namespace simulator {
namespace {

using namespace std::placeholders;

TwiMessage ParseTwiMessage(uint32_t value) {
  TwiMessageIrq v;
  v.u.v = value;
  return v.u.twi;
}

}  // namespace

I2cEeprom::I2cEeprom(Simavr *simavr, uint32_t size_bytes, uint8_t address,
                     uint8_t address_mask)
    : simavr_(simavr),
      size_bytes_(size_bytes),
      address_(address),
      address_mask_(address_mask) {
  // Reserve and write all 1's to the EEPROM, which is the default initial
  // EEPROM configuration from the factory.
  buffer_.reserve(size_bytes_);
  std::fill(buffer_.begin(), buffer_.end(), 0xFF);

  // Pre-calculate how many bytes are required to store the size of this EEPROM.
  // This is used to determine how many offset bytes are sent at the beginning
  // of each new transaction.
  num_offset_bytes_ = 0;
  do {
    size_bytes >>= 8;
    num_offset_bytes_++;
  } while (size_bytes);

  i2c_message_callback_ = std::make_unique<I2cMessageCallback>(
      std::bind(&I2cEeprom::HandleI2cMessage, this, _1));
  i2c_message_lifetime_ =
      simavr_->RegisterI2cMessageCallback(i2c_message_callback_.get());
}

bool I2cEeprom::IsRelevant(const TwiMessage &message) const {
  return (message.addr & address_mask_) == (address_ & address_mask_);
}

void I2cEeprom::HandleI2cMessage(uint32_t value) {
  TwiMessage message = ParseTwiMessage(value);
  LOG("I2cEeprom::HandleI2cMessage: %d", value);

  if (message.msg & TWI_COND_STOP) {
    // Reset the transaction if the message is addressed to us.
    if (IsRelevant(message)) {
      in_active_txn_ = false;
    }
  }
  if (message.msg & TWI_COND_START) {
    // On start, check if the message is addressed to us, otherwise reset.
    if (IsRelevant(message)) {
      in_active_txn_ = true;
      offset_ = 0;
      offset_index_ = 0;
      simavr_->RaiseI2cIrq(TWI_IRQ_INPUT,
                           simavr_->TwiIrqMsg(TWI_COND_ACK, true, 1));
    }
  }

  if (in_active_txn_) {
    if (message.msg & TWI_COND_WRITE) {
      // Write messages can serve one of two purposes:
      // 1. Writing the start index of the read/write transaction.
      // 2. Writing data to the EEPROM as part of an already indexed write
      //    transaction.
      // Index writing always happens before any read/writes occur.
      if (offset_index_ < num_offset_bytes_) {
        offset_ |= message.data << (offset_index_ * 8);
        offset_index_++;
      } else {
        buffer_[offset_++] = message.data;
      }

      // Ack the message.
      simavr_->RaiseI2cIrq(TWI_IRQ_INPUT,
                           simavr_->TwiIrqMsg(TWI_COND_ACK, true, 1));
    }

    if (message.msg & TWI_COND_READ) {
      // Simple return of selected byte.
      uint8_t current_byte = buffer_[offset_++];
      simavr_->RaiseI2cIrq(
          TWI_IRQ_INPUT, simavr_->TwiIrqMsg(TWI_COND_READ, true, current_byte));
    }
  }
}
}  // namespace simulator
}  // namespace threeboard