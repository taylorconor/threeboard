#pragma once

#include <array>
#include <cstdint>

#include "simulator/simavr/simavr.h"
#include "simulator/util/state_storage.h"

namespace threeboard {
namespace simulator {

using I2cMessageCallback = std::function<void(uint32_t)>;

// This class represents a simulated external EEPROM communicating with the main
// MCU via i2c.
class I2cEeprom {
 public:
  enum class Instance {
    EEPROM_0 = 0,
    EEPROM_1 = 3,
  };

  I2cEeprom(Simavr *simavr, StateStorage *state_storage, Instance instance);

 private:
  // Determine if the provided message is addressed to this EEPROM.
  bool IsRelevant(const TwiMessage &message) const;

  // Handle a message fragment.
  void HandleI2cMessage(uint32_t value);

  // Reset the state of this device.
  void Reset();

  Simavr *simavr_;

  // The hardware address of this EEPROM.
  const uint8_t address_;

  // Storage for the data in this EEPROM.
  std::array<uint8_t, 65535> buffer_;

  // The current state of the EEPROM. Since device addressing for the 22LC512
  // takes three bytes (and therefore three messages), we need to keep track of
  // the state of the device as it progresses through the device addressing
  // sequence.
  enum EepromState {
    STOPPED,
    ADDRESSING_HIGH,
    ADDRESSING_LOW,
    STARTED,
  };
  EepromState state_;

  // The read/write flag is sent in the control byte, and sets the mode of the
  // device for subsequent messages. We need to persist it for the duration of
  // the operation.
  enum EepromMode { WRITE, READ };
  EepromMode mode_;

  // The address of the current operation. This is only fully valid when
  // state_ == STARTED.
  uint16_t operation_address_;

  // Maintain the address of the last word accessed.
  uint8_t operation_address_counter_;

  std::unique_ptr<I2cMessageCallback> i2c_message_callback_;
  std::unique_ptr<Lifetime> i2c_message_lifetime_;
};
}  // namespace simulator
}  // namespace threeboard
