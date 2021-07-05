#include "storage_controller.h"

#include "src/logging.h"
#include "src/storage/internal/i2c_eeprom.h"
#include "src/storage/internal/internal_eeprom.h"
#include "src/util/util.h"

namespace threeboard {
namespace storage {

constexpr uint16_t kInternalEepromBaseOffset = 1024 - 256;

StorageController::StorageController(native::Native *native) {
  // Enable the MCUs TWI module before constructing the I2cEeprom instances, so
  // they don't need to configure this themselves and duplicate the logic.

  // Set the TWI prescaler to 0.
  native->SetTWSR(native->GetTWSR() & ~3);
  // Set the SCL clock frequency for the TWI interface to 100kHz.
  // native->SetTWBR(((F_CPU / 100000) - 16) / 2);
  native->SetTWBR(0xC);
  native->SetTWCR(1 << native::TWEN);

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

bool StorageController::SetCharacterShortcut(uint8_t index, uint8_t character) {
  character = character - 1;
  return external_eeprom_0_->Write(kInternalEepromBaseOffset + index,
                                   &character, 1);
}

bool StorageController::GetCharacterShortcut(uint8_t index, uint8_t *output) {
  LOG("StorageController::GetCharacterShortcut: idx=%d", index);
  uint8_t character = 0;
  RETURN_IF_ERROR(external_eeprom_0_->Read(kInternalEepromBaseOffset + index,
                                           &character, 1));
  // On first boot, the EEPROM has 0xFF written at every byte. Rather than
  // wasting time (and risking corruption) by rewriting all of these bytes to
  // zero on first boot, instead we represent the stored shortcut as
  // shortcut_val - 1. That means a byte entry of 0xFF represents a shortcut of
  // value 0.
  LOG("StorageController::GetCharacterShortcut: character read: %d",
      character + 1);
  *output = character + 1;
  return true;
}

bool StorageController::AppendToWordShortcut(uint8_t index, uint8_t character) {
  // manifest_->GetWordShortcutIndex(
  //     logical_index, &physical_index, &physical_eeprom);
  return true;
}

bool StorageController::DeleteWordShortcut(uint8_t index) { return true; }
bool StorageController::GetWordShortcutLength(uint8_t index, uint8_t *output) {
  return true;
}
bool StorageController::SendWordShortcut(uint8_t index) { return true; }

}  // namespace storage
}  // namespace threeboard