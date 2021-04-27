#include "storage_controller.h"

#include "src/storage/internal/i2c_eeprom.h"
#include "src/storage/internal/internal_eeprom.h"
#include "src/util/util.h"

namespace threeboard {
namespace storage {

constexpr uint16_t kInternalEepromBaseOffset = 1024 - 256;

StorageController::StorageController(native::Native *native) {
  static InternalEeprom internal_eeprom(native);
  static I2cEeprom external_eeprom_0(native, I2cEeprom::EEPROM_0);
  static I2cEeprom external_eeprom_1(native, I2cEeprom::EEPROM_1);
  internal_eeprom_ = &internal_eeprom;
  external_eeprom_0_ = &external_eeprom_0;
  external_eeprom_1_ = &external_eeprom_1;
}

// Test-only, protected constructor for injecting mock EEPROM instances during
// testing.
StorageController::StorageController(Eeprom *internal_eeprom,
                                     Eeprom *external_eeprom_0,
                                     Eeprom *external_eeprom_1)
    : internal_eeprom_(internal_eeprom),
      external_eeprom_0_(external_eeprom_0),
      external_eeprom_1_(external_eeprom_1) {}

bool StorageController::InitializeManifest() {
  RETURN_IF_ERROR(internal_eeprom_->Read(kInternalEepromBaseOffset,
                                         character_shortcut_buf_, 256));
  return true;
}

void StorageController::SetCharacterShortcut(uint8_t offset,
                                             uint8_t character) {
  if (character_shortcut_buf_[offset] != character) {
    internal_eeprom_->Write(kInternalEepromBaseOffset + offset, &character, 1);
    character_shortcut_buf_[offset] = character;
  }
}

uint8_t StorageController::GetCharacterShortcut(uint8_t offset) {
  return character_shortcut_buf_[offset];
}

}  // namespace storage
}  // namespace threeboard