#pragma once

#include "eeprom.h"
#include "gmock/gmock.h"

namespace threeboard {
namespace storage {

class EepromMockDefault : public Eeprom {
 public:
  MOCK_METHOD(bool, ReadByte, (const uint16_t &, uint8_t *), (override));
  MOCK_METHOD(bool, WriteByte, (const uint16_t &, uint8_t), (override));
};

using EepromMock = ::testing::StrictMock<EepromMockDefault>;

}  // namespace storage
}  // namespace threeboard