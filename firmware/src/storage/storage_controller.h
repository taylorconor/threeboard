#pragma once

#include "src/native/native.h"
#include "src/storage/internal/eeprom.h"

namespace threeboard {
namespace storage {

class StorageController {
 public:
  explicit StorageController(native::Native *native);

  // This method MUST be called after enabling interrupts, but before any
  // get/set methods interacting with the EEPROMs in this class. Otherwise these
  // operations are undefined.
  bool InitializeManifest();

  void SetCharacterShortcut(uint8_t offset, uint8_t character);
  uint8_t GetCharacterShortcut(uint8_t offset);

 protected:
  // Test-only.
  StorageController(Eeprom *internal_eeprom, Eeprom *external_eeprom_0,
                    Eeprom *external_eeprom_1);

 private:
  Eeprom *internal_eeprom_;
  Eeprom *external_eeprom_0_;
  Eeprom *external_eeprom_1_;
};

}  // namespace storage
}  // namespace threeboard
