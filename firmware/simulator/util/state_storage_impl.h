#pragma once

#include "absl/status/statusor.h"
#include "nlohmann/json.hpp"
#include "state_storage.h"

namespace threeboard {
namespace simulator {

// A class to facilitate parsing of and interactions with the simulator's state
// file on disk.
class StateStorageImpl : public StateStorage {
 public:
  static absl::StatusOr<std::unique_ptr<StateStorage>> CreateFromFile(
      const std::string& file_path);
  ~StateStorageImpl();

  std::array<uint8_t, 1024>* GetInternalEepromData() override {
    return &internal_eeprom_;
  }
  std::array<uint8_t, 65535>* GetEeprom0Data() override { return &eeprom0_; }
  std::array<uint8_t, 65535>* GetEeprom1Data() override { return &eeprom1_; }

 protected:
  StateStorageImpl(const std::string& file_path, const nlohmann::json& json);

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