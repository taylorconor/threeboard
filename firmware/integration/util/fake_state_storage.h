#pragma once

#include "simulator/util/state_storage.h"

namespace threeboard {
namespace integration {

// A test-only StateStorage implementation used for tests where a real
// StateStorageImpl setup is not needed.
class FakeStateStorage : public simulator::StateStorage {
 public:
  FakeStateStorage() {
    internal_data_.fill(0xFF);
    eeprom0_data_.fill(0xFF);
    eeprom1_data_.fill(0xFF);
  }

  simulator::InternalEepromData* GetInternalEepromData() {
    return &internal_data_;
  }
  simulator::Eeprom0Data* GetEeprom0Data() { return &eeprom0_data_; }
  simulator::Eeprom1Data* GetEeprom1Data() { return &eeprom1_data_; }

 private:
  simulator::InternalEepromData internal_data_;
  simulator::Eeprom0Data eeprom0_data_;
  simulator::Eeprom1Data eeprom1_data_;
};

}  // namespace integration
}  // namespace threeboard
