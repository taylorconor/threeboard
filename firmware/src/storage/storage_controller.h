#pragma once

#include "src/storage/internal/eeprom.h"

namespace threeboard {
namespace storage {

class StorageController {
 public:
  StorageController(Eeprom *internal_eeprom, Eeprom *external_eeprom_1,
                    Eeprom *external_eeprom_2);

 private:
  Eeprom *internal_eeprom_;
  Eeprom *external_eeprom_1_;
  Eeprom *external_eeprom_2_;
};

}  // namespace storage
}  // namespace threeboard
