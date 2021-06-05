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

bool StorageController::InitializeManifest() { return true; }

void StorageController::SetCharacterShortcut(uint8_t offset,
                                             uint8_t character) {
  character = character - 1;
  internal_eeprom_->Write(kInternalEepromBaseOffset + offset, &character, 1);
}

uint8_t StorageController::GetCharacterShortcut(uint8_t offset) {
  uint8_t character = 0;
  internal_eeprom_->Read(kInternalEepromBaseOffset + offset, &character, 1);
  // On first boot, the EEPROM has 0xFF written at every byte. Rather than
  // wasting time (and risking corruption) by rewriting all of these bytes to
  // zero on first boot, instead we represent the stored shortcut as
  // shortcut_val - 1. That means a byte entry of 0xFF represents a shortcut of
  // value 0.
  return character + 1;
}

}  // namespace storage
}  // namespace threeboard