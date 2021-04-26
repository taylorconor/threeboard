#include "storage_controller.h"

namespace threeboard {
namespace storage {

StorageController::StorageController(Eeprom *internal_eeprom,
                                     Eeprom *external_eeprom_1,
                                     Eeprom *external_eeprom_2)
    : internal_eeprom_(internal_eeprom),
      external_eeprom_1_(external_eeprom_1),
      external_eeprom_2_(external_eeprom_2) {}

}  // namespace storage
}  // namespace threeboard