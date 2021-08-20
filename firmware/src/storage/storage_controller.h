#pragma once

#include "src/native/native.h"
#include "src/storage/internal/eeprom.h"
#include "src/usb/usb_controller.h"

namespace threeboard {
namespace storage {

class StorageController {
 public:
  StorageController(native::Native *native, usb::UsbController *usb_controller);

  bool SetCharacterShortcut(uint8_t index, uint8_t character);
  bool GetCharacterShortcut(uint8_t index, uint8_t *output);

  bool AppendToWordShortcut(uint8_t index, uint8_t character);
  bool ClearWordShortcut(uint8_t index);
  bool GetWordShortcutLength(uint8_t index, uint8_t *output);
  bool SendWordShortcut(uint8_t index, uint8_t word_mod_code);

  bool AppendToBlobShortcut(uint8_t index, uint8_t character, uint8_t modcode);
  bool ClearBlobShortcut(uint8_t index);
  bool GetBlobShortcutLength(uint8_t index, uint8_t *output);
  bool SendBlobShortcut(uint8_t index);

 protected:
  // Test-only.
  StorageController(usb::UsbController *usb_controller, Eeprom *internal_eeprom,
                    Eeprom *external_eeprom_0, Eeprom *external_eeprom_1);

 private:
  usb::UsbController *usb_controller_;
  Eeprom *internal_eeprom_;
  Eeprom *external_eeprom_0_;
  Eeprom *external_eeprom_1_;
};

}  // namespace storage
}  // namespace threeboard
