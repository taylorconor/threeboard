#include "i2c_eeprom.h"

#include <functional>

#include "simavr/avr_twi.h"
#include "src/logging.h"

namespace threeboard {
namespace simulator {
namespace {

using namespace std::placeholders;

static const char *_ee_irq_names[2] = {
    [TWI_IRQ_INPUT] = "8>eeprom.out",
    [TWI_IRQ_OUTPUT] = "32<eeprom.in",
};

avr_twi_msg_t ParseTwiMessage(uint32_t value) {
  avr_twi_msg_irq_t v;
  v.u.v = value;
  return v.u.twi;
}

void i2c_eeprom_in_hook(struct avr_irq_t *, uint32_t value, void *param) {
  auto *callback = (I2cMessageCallback *)param;
  (*callback)(value);
}

} // namespace

I2cEeprom::I2cEeprom(Simavr *simavr, uint32_t size_bytes, uint8_t address,
                     uint8_t address_mask)
    : simavr_(simavr), size_bytes_(size_bytes), address_(address),
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

  // Allocate and wire up the IRQs needed for IO.
  irq_ = avr_alloc_irq(&simavr_->GetAvr()->irq_pool, 0, 2, _ee_irq_names);
  callback_ = std::bind(&I2cEeprom::HandleI2cMessage, this, _1);
  avr_irq_register_notify(irq_ + TWI_IRQ_OUTPUT, i2c_eeprom_in_hook,
                          &callback_);

  avr_connect_irq(
      irq_ + TWI_IRQ_INPUT,
      avr_io_getirq(simavr_->GetAvr(), AVR_IOCTL_TWI_GETIRQ(0), TWI_IRQ_INPUT));
  avr_connect_irq(
      avr_io_getirq(simavr_->GetAvr(), AVR_IOCTL_TWI_GETIRQ(0), TWI_IRQ_OUTPUT),
      irq_ + TWI_IRQ_OUTPUT);
}

bool I2cEeprom::IsRelevant(const avr_twi_msg_t &message) {
  return (message.addr & address_mask_) == (address_ & address_mask_);
}

void I2cEeprom::HandleI2cMessage(uint32_t value) {
  avr_twi_msg_t message = ParseTwiMessage(value);

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
      avr_raise_irq(irq_ + TWI_IRQ_INPUT,
                    avr_twi_irq_msg(TWI_COND_ACK, true, 1));
    }
  }

  if (in_active_txn_) {
    if (message.msg & TWI_COND_WRITE) {
      // Write messages can serve two purposes:
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
      // TODO: does the data value matter here?
      avr_raise_irq(irq_ + TWI_IRQ_INPUT,
                    avr_twi_irq_msg(TWI_COND_ACK, true, 1));
    }
    if (message.msg & TWI_COND_READ) {
      // Simple return of selected byte.
      uint8_t current_byte = buffer_[offset_++];
      avr_raise_irq(irq_ + TWI_IRQ_INPUT,
                    avr_twi_irq_msg(TWI_COND_READ, true, current_byte));
    }
  }
}
} // namespace simulator
} // namespace threeboard