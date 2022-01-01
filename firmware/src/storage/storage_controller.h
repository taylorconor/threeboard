#pragma once

#include "src/native/native.h"
#include "src/storage/internal/eeprom.h"
#include "src/usb/usb_controller.h"

namespace threeboard {
namespace storage {

enum class WordModCode {
  LOWERCASE = 0,
  UPPERCASE = 1,
  CAPITALISE = 2,
  APPEND_PERIOD = 3,
  APPEND_COMMA = 4,
  APPEND_HYPHEN = 5,
};

// Abstracts away interactions with the various storage devices on the
// threeboard. This class controls the layout of storage, interfaces with the
// storage devices, and provides a human-readable C++ abstraction on top of
// them.
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
  // Allow derived classes (StorageControllerMock) to skip the initialising
  // constructor.
  StorageController() {}

 private:
  friend class StorageControllerTest;

  // Test-only constructor for injecting mock UsbController and Eeprom
  // instances.
  StorageController(usb::UsbController *usb_controller, Eeprom *internal_eeprom,
                    Eeprom *external_eeprom_0, Eeprom *external_eeprom_1)
      : usb_controller_(usb_controller),
        internal_eeprom_(internal_eeprom),
        external_eeprom_0_(external_eeprom_0),
        external_eeprom_1_(external_eeprom_1) {}

  usb::UsbController *usb_controller_;
  Eeprom *internal_eeprom_;
  Eeprom *external_eeprom_0_;
  Eeprom *external_eeprom_1_;
};

}  // namespace storage
}  // namespace threeboard
