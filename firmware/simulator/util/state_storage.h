#pragma once

#include "absl/status/statusor.h"
#include "nlohmann/json.hpp"

namespace threeboard {
namespace simulator {

using InternalEepromData = std::array<uint8_t, 1024>;
using Eeprom0Data = std::array<uint8_t, 65535>;
using Eeprom1Data = std::array<uint8_t, 65535>;

// A class to facilitate parsing of and interactions with the simulator's state
// file on disk.
class StateStorage {
 public:
  static absl::StatusOr<std::unique_ptr<StateStorage>> CreateFromFile(
      const std::string& file_path);
  ~StateStorage();

  std::array<uint8_t, 1024>* GetInternalEepromData() {
    return &internal_eeprom_;
  }
  std::array<uint8_t, 65535>* GetEeprom0Data() { return &eeprom0_; }
  std::array<uint8_t, 65535>* GetEeprom1Data() { return &eeprom1_; }

 protected:
  StateStorage(const std::string& file_path, const nlohmann::json& json);

  // Methods for parsing the shortcut file into their allocated buffer.
  void ConfigureInternalEeprom();
  void ConfigureEeprom0();
  void ConfigureEeprom1();

  std::string file_path_;
  nlohmann::json json_;
  std::array<uint8_t, 1024> internal_eeprom_;
  std::array<uint8_t, 65535> eeprom0_;
  std::array<uint8_t, 65535> eeprom1_;
};

}  // namespace simulator
}  // namespace threeboard