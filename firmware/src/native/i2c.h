#pragma once

#include "src/native/native.h"

namespace threeboard {
namespace native {

class I2C {
 public:
  explicit I2C(native::Native *native);

  // TODO: return status?
  void Write(uint8_t address, const uint32_t &offset, uint8_t *data,
             const uint32_t &length);
  void Read(uint8_t address, const uint32_t &offset, uint8_t *data,
            const uint32_t &length);

 private:
  native::Native *native_;

  void SendStart();
  void SendStop();
  void WriteByte(uint8_t data);
  uint8_t ReadByte();
  void StartTransaction(uint8_t address, uint8_t mask, const uint32_t &offset);
  void StartWriteTransaction(uint8_t address, const uint32_t &offset);
  void StartReadTransaction(uint8_t address, const uint32_t &offset);
};
}  // namespace native
}  // namespace threeboard
