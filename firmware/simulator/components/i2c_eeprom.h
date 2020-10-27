#pragma once

#include <cstdint>
#include <simavr/avr_twi.h>
#include <vector>

#include "simavr/sim_irq.h"
#include "simulator/simavr/simavr.h"

namespace threeboard {
namespace simulator {

using I2cMessageCallback = std::function<void(uint32_t)>;

class I2cEeprom {
public:
  I2cEeprom(Simavr *simavr, uint32_t size_bytes, uint8_t address,
            uint8_t address_mask);

private:
  bool IsRelevant(const avr_twi_msg_t &message);
  void HandleI2cMessage(uint32_t value);

  Simavr *simavr_;
  // Byte size of this EEPROM.
  uint32_t size_bytes_;
  // The hardware address of this EEPROM.
  uint8_t address_;
  uint8_t address_mask_;
  I2cMessageCallback callback_;
  std::vector<uint8_t> buffer_;
  // True if this EEPROM is currently addressed in a transaction.
  bool in_active_txn_;
  // Number of bytes required to store a read/write offset in this EEPROM. e.g.
  // a 512K EEPROM needs 19 bits (= 3 bytes) to store its offset.
  uint8_t num_offset_bytes_;
  // The read/write offset specified in this transaction (only valid if
  // in_active_txn_ is set).
  uint32_t offset_;
  // The current index into num_offset_bytes_, used when receiving the multibyte
  // offset one byte at a time.
  uint8_t offset_index_;
  avr_irq_t *irq_;
};
} // namespace simulator
} // namespace threeboard
