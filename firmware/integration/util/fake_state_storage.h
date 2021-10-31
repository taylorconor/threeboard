#pragma once

#include "simulator/util/state_storage.h"

namespace threeboard {
namespace simulator {

class FakeStateStorage : public StateStorage {
 public:
  FakeStateStorage() {
    internal_data_.fill(0xFF);
    eeprom0_data_.fill(0xFF);
    eeprom1_data_.fill(0xFF);
  }

  InternalEepromData* GetInternalEepromData() { return &internal_data_; }
  Eeprom0Data* GetEeprom0Data() { return &eeprom0_data_; }
  Eeprom1Data* GetEeprom1Data() { return &eeprom1_data_; }

 private:
  InternalEepromData internal_data_;
  Eeprom0Data eeprom0_data_;
  Eeprom1Data eeprom1_data_;
};

}  // namespace simulator
}  // namespace threeboard
