#include "state_storage.h"

#include <fstream>

#include "util/status_util.h"

namespace threeboard {
namespace simulator {
namespace {

struct JsonProperty {
  // The name of this property section in the file.
  std::string name;
  // The maximum amount of shortcuts allowed for this property.
  uint8_t max_index;
  // The maximum length allowed for each shortcut.
  uint16_t max_length;
};

const std::vector<JsonProperty> json_properties = {
    {"character_shortcuts", 255, 1},
    {"word_shortcuts", 255, 16},
    {"blob_shortcuts", 240, 255}};

// Ensure that the provided json (parsed from the state file) is valid, and if
// not provide verbose error messages, as they will be user-facing.
absl::Status Validate(const nlohmann::json &json) {
  for (const auto &property : json_properties) {
    if (!json.contains(property.name)) {
      return absl::InvalidArgumentError(absl::StrCat(
          "State file must contain property '", property.name, "'"));
    }
    for (const auto &[raw_idx, raw_value] : json[property.name].items()) {
      int idx;
      std::string value;
      try {
        idx = std::stoi(raw_idx);
      } catch (const std::exception &e) {
        return absl::InvalidArgumentError(absl::StrCat(
            "Could not convert index '", raw_idx, "' to int: ", e.what()));
      }
      try {
        value = raw_value.get<std::string>();
      } catch (const std::exception &e) {
        return absl::InvalidArgumentError(absl::StrCat(
            "The value for index ", idx, " must be a string: ", e.what()));
      }

      if (0 > idx || idx > property.max_index) {
        return absl::InvalidArgumentError(absl::StrCat(
            "Index ", idx, " is out of range for property '", property.name,
            "'. The largest allowable index for this property is ",
            property.max_index));
      }
      if (value.length() > property.max_length) {
        return absl::InvalidArgumentError(absl::StrCat(
            "Value '", value, "' at index ", idx, " in property '",
            property.name, "' exceeds the property's max length of ",
            property.max_length));
      }
    }
  }
  return absl::OkStatus();
}

}  // namespace

// static.
absl::StatusOr<std::unique_ptr<StateStorage>> StateStorage::CreateFromFile(
    const std::string &file_path) {
  std::ifstream input_stream(file_path);
  nlohmann::json json;
  try {
    json = nlohmann::json::parse(input_stream);
  } catch (const std::exception &e) {
    return absl::InternalError(e.what());
  }
  RETURN_IF_ERROR(Validate(json));
  auto *raw_ptr = new StateStorage(file_path, json);
  return std::unique_ptr<StateStorage>(raw_ptr);
}

StateStorage::StateStorage(const std::string &file_path,
                           const nlohmann::json &json)
    : file_path_(file_path), json_(json) {}

void StateStorage::ConfigureInternalEeprom(
    std::array<uint8_t, 1024> *internal_eeprom) {
  internal_eeprom->fill(0xFF);
  for (const auto &[idx, value] : json_["character_shortcuts"].items()) {
    // TODO: remove this hack and implement proper USB keycode conversion.
    char c = value.get<std::string>()[0] - 'a' + 4 - 1;
    internal_eeprom->at(std::stoi(idx)) = c;
  }
  for (const auto &[idx, value] : json_["word_shortcuts"].items()) {
    internal_eeprom->at(256 + std::stoi(idx)) =
        value.get<std::string>().length() - 1;
  }
}

void StateStorage::ConfigureEeprom0(std::array<uint8_t, 65535> *eeprom0) {
  eeprom0->fill(0xFF);
  for (const auto &[idx, raw_value] : json_["word_shortcuts"].items()) {
    auto value = raw_value.get<std::string>();
    for (int i = 0; i < value.length(); ++i) {
      // TODO: remove this hack and implement proper USB keycode conversion.
      char c = value[i] - 'a' + 4 - 1;
      eeprom0->at((std::stoi(idx) * 16) + i) = c;
    }
  }
}

void StateStorage::ConfigureEeprom1(std::array<uint8_t, 65535> *eeprom1) {
  eeprom1->fill(0xFF);
  for (const auto &[idx, value] : json_["blob_shortcuts"].items()) {
  }
}

}  // namespace simulator
}  // namespace threeboard