#pragma once

namespace threeboard {
namespace simulator {

using InternalEepromData = std::array<uint8_t, 1024>;
using Eeprom0Data = std::array<uint8_t, 65535>;
using Eeprom1Data = std::array<uint8_t, 65535>;

class StateStorage {
 public:
  virtual ~StateStorage() = default;

  virtual InternalEepromData* GetInternalEepromData() = 0;
  virtual Eeprom0Data* GetEeprom0Data() = 0;
  virtual Eeprom1Data* GetEeprom1Data() = 0;
};

}  // namespace simulator
}  // namespace threeboard