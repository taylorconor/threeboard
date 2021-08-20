#include "storage_controller.h"

#include "src/logging.h"
#include "src/native/mcu.h"
#include "src/storage/internal/i2c_eeprom.h"
#include "src/storage/internal/internal_eeprom.h"
#include "src/util/util.h"

namespace threeboard {
namespace storage {
namespace {

// Definitions of some constants used for indexing into the StorageController's
// underlying EEPROM storage devices.
//
// The internal EEPROM contains the character shortcut storage, as well as
// metadata storage for the other layers (specifically, the length of each word
// or blob shortcut for layers G and B). It is laid out as follows:
// |- char shortcuts -| |- layer G lengths -| |- layer B lengths -| |- unused -|
// |------ 256 B -----| |------ 256 B ------| |------ ??? B ------| |-- ??? B -|
//                      ^                     ^
//                    0x100                 0x200
// |--------------------- internal EEPROM size = 1024 B -----------------------|

constexpr uint16_t kInternalEepromLayerGLengthStart = 0x100;
constexpr uint16_t kInternalEepromLayerBLengthStart = 0x200;

constexpr uint8_t kWordModUppercase = 0;
constexpr uint8_t kWordModCapitalise = 1;

enum class WordModCode {
  LOWERCASE = 0,
  UPPERCASE = 1,
  CAPITALISE = 2,
  APPEND_PERIOD = 3,
  APPEND_COMMA = 4,
  APPEND_HYPHEN = 5,
};

}  // namespace

StorageController::StorageController(native::Native *native,
                                     usb::UsbController *usb_controller)
    : usb_controller_(usb_controller) {
  // Set the TWI prescaler to 0.
  native->SetTWSR(native->GetTWSR() & ~3);
  // Set the SCL clock frequency for the TWI interface to 100kHz.
  native->SetTWBR(((F_CPU / 100000) - 16) / 2);

  // Enable the MCUs TWI module before constructing the I2cEeprom instances, so
  // they don't need to configure this themselves and duplicate the logic.
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
StorageController::StorageController(usb::UsbController *usb_controller,
                                     Eeprom *internal_eeprom,
                                     Eeprom *external_eeprom_0,
                                     Eeprom *external_eeprom_1)
    : usb_controller_(usb_controller),
      internal_eeprom_(internal_eeprom),
      external_eeprom_0_(external_eeprom_0),
      external_eeprom_1_(external_eeprom_1) {}

bool StorageController::SetCharacterShortcut(uint8_t index, uint8_t character) {
  return internal_eeprom_->WriteByte(index, character);
}

bool StorageController::GetCharacterShortcut(uint8_t index, uint8_t *output) {
  return internal_eeprom_->ReadByte(index, output);
}

bool StorageController::AppendToWordShortcut(uint8_t index, uint8_t character) {
  uint8_t length;
  RETURN_IF_ERROR(GetWordShortcutLength(index, &length));
  // If this shortcut slot is already full (16 characters) then we need to
  // propagate an error.
  if (length == 15) {
    return false;
  }
  RETURN_IF_ERROR(
      external_eeprom_0_->WriteByte((index * 16) + length, character));
  return internal_eeprom_->WriteByte(kInternalEepromLayerGLengthStart + index,
                                     length + 1);
}

bool StorageController::ClearWordShortcut(uint8_t index) {
  return internal_eeprom_->WriteByte(kInternalEepromLayerGLengthStart + index,
                                     0);
}

bool StorageController::GetWordShortcutLength(uint8_t index, uint8_t *output) {
  return internal_eeprom_->ReadByte(kInternalEepromLayerGLengthStart + index,
                                    output);
}

bool StorageController::SendWordShortcut(uint8_t index, uint8_t raw_mod_code) {
  WordModCode word_mod_code = (WordModCode)raw_mod_code;
  uint8_t length;
  RETURN_IF_ERROR(GetWordShortcutLength(index, &length));
  // If this shortcut slot is empty then we should propagate an error instead of
  // doing nothing.
  if (length == 0) {
    return false;
  }
  for (int i = 0; i < length; ++i) {
    uint8_t character;
    RETURN_IF_ERROR(external_eeprom_0_->ReadByte((index * 16) + i, &character));
    if (word_mod_code == WordModCode::UPPERCASE ||
        (word_mod_code == WordModCode::CAPITALISE && i == 0)) {
      RETURN_IF_ERROR(usb_controller_->SendKeypress(character, (1 << 1)));
    } else {
      if (i == length - 1) {
        uint8_t append = 0;
        if (word_mod_code == WordModCode::APPEND_PERIOD) {
          append = 0x37;
        } else if (word_mod_code == WordModCode::APPEND_COMMA) {
          append = 0x36;
        } else if (word_mod_code == WordModCode::APPEND_HYPHEN) {
          append = 0x2d;
        }
        if (append > 0) {
          RETURN_IF_ERROR(usb_controller_->SendKeypress(character, 0));
          character = append;
        }
      }
      RETURN_IF_ERROR(usb_controller_->SendKeypress(character, 0));
    }
  }
  return true;
}

bool StorageController::AppendToBlobShortcut(uint8_t index, uint8_t character,
                                             uint8_t modcode) {
  return true;
}

bool StorageController::ClearBlobShortcut(uint8_t index) { return true; }

bool StorageController::GetBlobShortcutLength(uint8_t index, uint8_t *output) {
  return true;
}

bool StorageController::SendBlobShortcut(uint8_t index) { return true; }

}  // namespace storage
}  // namespace threeboard