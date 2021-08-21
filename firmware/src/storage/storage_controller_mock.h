#pragma once

#include "gmock/gmock.h"
#include "src/storage/storage_controller.h"

namespace threeboard {
namespace storage {
class StorageControllerMockDefault : public StorageController {
 public:
  MOCK_METHOD(bool, SetCharacterShortcut, (uint8_t, uint8_t), (override));
  MOCK_METHOD(bool, GetCharacterShortcut, (uint8_t, uint8_t *), (override));

  MOCK_METHOD(bool, AppendToWordShortcut, (uint8_t, uint8_t), (override));
  MOCK_METHOD(bool, ClearWordShortcut, (uint8_t), (override));
  MOCK_METHOD(bool, GetWordShortcutLength, (uint8_t, uint8_t *), (override));
  MOCK_METHOD(bool, SendWordShortcut, (uint8_t, uint8_t), (override));

  MOCK_METHOD(bool, AppendToBlobShortcut, (uint8_t, uint8_t, uint8_t),
              (override));
  MOCK_METHOD(bool, ClearBlobShortcut, (uint8_t), (override));
  MOCK_METHOD(bool, GetBlobShortcutLength, (uint8_t, uint8_t *), (override));
  MOCK_METHOD(bool, SendBlobShortcut, (uint8_t), (override));
};

using StorageControllerMock =
    ::testing::StrictMock<StorageControllerMockDefault>;

}  // namespace storage
}  // namespace threeboard
