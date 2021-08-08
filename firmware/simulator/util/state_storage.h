#pragma once

#include "absl/status/statusor.h"
#include "nlohmann/json.hpp"

namespace threeboard {
namespace simulator {

// A class to facilitate parsing of and interactions with the simulator's state
// file on disk.
class StateStorage {
 public:
  static absl::StatusOr<std::unique_ptr<StateStorage>> CreateFromFile(
      const std::string &file_path);

  // Methods for parsing the shortcut file into an allocated target buffer.
  void ConfigureInternalEeprom(std::array<uint8_t, 1024> *internal_eeprom);
  void ConfigureEeprom0(std::array<uint8_t, 65535> *eeprom0);
  void ConfigureEeprom1(std::array<uint8_t, 65535> *eeprom1);

  // TODO: Methods for modifying the shortcut file in response to user
  // interaction with the simulator.

 private:
  StateStorage(const std::string &file_path, const nlohmann::json &json);

  std::string file_path_;
  nlohmann::json json_;
};

}  // namespace simulator
}  // namespace threeboard