#include "i2c_eeprom.h"

#include <functional>
#include <thread>

#include "simulator/util/logging.h"

namespace threeboard {
namespace simulator {
namespace {

using namespace std::placeholders;

// 24LC512 control code.
constexpr uint8_t kControlCode = 0b10100000;

TwiMessage ParseTwiMessage(uint32_t value) {
  TwiMessageIrq v;
  v.u.v = value;
  return v.u.twi;
}

uint8_t ParseMessageAddress(uint8_t address) { return (address & 14) >> 1; }

std::string GetMessageType(const TwiMessage &message) {
  std::string str;
  if (message.msg & TWI_COND_STOP) {
    str += "STOP";
  }
  if (message.msg & TWI_COND_START) {
    str += "START";
  }
  if (message.msg & TWI_COND_WRITE) {
    str += "WRITE";
  }
  if (message.msg & TWI_COND_READ) {
    str += "READ";
  }
  return str;
}

bool IsControlByte(const TwiMessage &message) {
  return message.addr & kControlCode;
}
}  // namespace

I2cEeprom::I2cEeprom(Simavr *simavr, StateStorage *state_storage,
                     Instance instance)
    : simavr_(simavr), address_((uint8_t)instance) {
  i2c_message_callback_ = std::make_unique<I2cMessageCallback>(
      std::bind(&I2cEeprom::HandleI2cMessage, this, _1));
  i2c_message_lifetime_ =
      simavr_->RegisterI2cMessageCallback(i2c_message_callback_.get());

  if (instance == Instance::EEPROM_0) {
    state_storage->ConfigureEeprom0(&buffer_);
  } else if (instance == Instance::EEPROM_1) {
    state_storage->ConfigureEeprom1(&buffer_);
  }

  Reset();
}

bool I2cEeprom::IsRelevant(const TwiMessage &message) const {
  return ParseMessageAddress(message.addr) == address_;
}

void I2cEeprom::HandleI2cMessage(uint32_t value) {
  TwiMessage message = ParseTwiMessage(value);
  // Perform a slight handling delay here to simulate potential delays within
  // the silicon.
  std::this_thread::sleep_for(std::chrono::milliseconds(5));

  if (!IsRelevant(message)) {
    // A start for a different device on the bus triggers a reset for this
    // device.
    if (message.msg & TWI_COND_START) {
      Reset();
    }
    return;
  }

  if (message.msg & TWI_COND_STOP) {
    state_ = STOPPED;
    Reset();
  }
  if (message.msg & TWI_COND_START) {
    if (!IsControlByte(message)) {
      // This should never be invoked in the threeboard code.
      LOG("SIM::HandleI2cMessage: Received non-control start message, "
          "returning");
      return;
    }
    LOG("SIM::HandleI2cMessage: Received control byte, mode=%s",
        mode_ == READ ? "READ" : "WRITE");

    // ACK the start message, parse out the read/write flag, and prepare to
    // receive the two address bytes.
    mode_ = static_cast<EepromMode>(message.addr & 1);

    // The address word should only be sent for write operations. Read
    // operations are addressed first by starting a write operation and writing
    // the address word, and then sending a repeated start to switch the device
    // into read operation mode once it has been addressed.
    if (mode_ == WRITE) {
      state_ = ADDRESSING_HIGH;
    } else {
      state_ = STARTED;
    }

    simavr_->RaiseI2cIrq(TWI_IRQ_INPUT,
                         simavr_->TwiIrqMsg(TWI_COND_ACK, address_, 1));
  }

  if (message.msg & TWI_COND_WRITE) {
    // Write messages can serve one of two purposes:
    // 1. Writing the address of the read/write operation.
    // 2. Writing data to the EEPROM as part of an already addressed write
    //    operation.
    // Address writing always happens before any write occurs.
    if (state_ == ADDRESSING_HIGH) {
      operation_address_ |= (message.data << 8);
      state_ = ADDRESSING_LOW;
    } else if (state_ == ADDRESSING_LOW) {
      operation_address_ |= message.data;
      state_ = STARTED;
    } else if (state_ == STARTED) {
      buffer_[operation_address_ + operation_address_counter_] = message.data;
      operation_address_counter_++;
    }

    // Ack the message.
    simavr_->RaiseI2cIrq(TWI_IRQ_INPUT,
                         simavr_->TwiIrqMsg(TWI_COND_ACK, address_, 1));
  }

  if (message.msg & TWI_COND_READ) {
    // Simple return of selected byte.
    uint8_t current_byte =
        buffer_[operation_address_ + operation_address_counter_];
    LOG("SIM::HandleI2cMessage: Responding to read at addr %d with byte %d",
        operation_address_ + operation_address_counter_, current_byte);
    operation_address_counter_++;
    simavr_->RaiseI2cIrq(
        TWI_IRQ_INPUT,
        simavr_->TwiIrqMsg(TWI_COND_READ, address_, current_byte));
  }
}

void I2cEeprom::Reset() {
  operation_address_ = 0;
  operation_address_counter_ = 0;
  state_ = STOPPED;
}
}  // namespace simulator
}  // namespace threeboard