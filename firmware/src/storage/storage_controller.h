#pragma once

#include "src/native/native.h"
#include "src/storage/internal/eeprom.h"
#include "src/usb/usb_controller.h"

namespace threeboard {
namespace storage {

class StorageController {
 public:
  StorageController(native::Native *native, usb::UsbController *usb_controller);
  virtual ~StorageController() {}

  virtual bool SetCharacterShortcut(uint8_t index, uint8_t character);
  virtual bool GetCharacterShortcut(uint8_t index, uint8_t *output);

  virtual bool AppendToWordShortcut(uint8_t index, uint8_t character);
  virtual bool ClearWordShortcut(uint8_t index);
  virtual bool GetWordShortcutLength(uint8_t index, uint8_t *output);
  virtual bool SendWordShortcut(uint8_t index, uint8_t word_mod_code);

  virtual bool AppendToBlobShortcut(uint8_t index, uint8_t character,
                                    uint8_t modcode);
  virtual bool ClearBlobShortcut(uint8_t index);
  virtual bool GetBlobShortcutLength(uint8_t index, uint8_t *output);
  virtual bool SendBlobShortcut(uint8_t index);

 protected:
  // Test-only constructors.
  StorageController(usb::UsbController *usb_controller, Eeprom *internal_eeprom,
                    Eeprom *external_eeprom_0, Eeprom *external_eeprom_1);
  StorageController() {}

 private:
  usb::UsbController *usb_controller_;
  Eeprom *internal_eeprom_;
  Eeprom *external_eeprom_0_;
  Eeprom *external_eeprom_1_;
};

}  // namespace storage
}  // namespace threeboard
