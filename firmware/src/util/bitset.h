#pragma once

#include <stdint.h>

namespace threeboard {
namespace util {

// a small stack-allocated bitset class, padded to the next word boundary.
template <uint8_t N, uint8_t W = (N / 8) + 1> class bitset {
public:
  // set a single bit `val` at `idx` in the bitfield.
  // NOTE: assumes valid `idx`.
  void SetBit(const uint8_t idx, bool val) {
    storage_[idx / 8] &= ~(1 << idx % 8);
    storage_[idx / 8] |= val << idx % 8;
  }

  // set a range of bits of size `size` in the bitfield, beginning at `idx`,
  // with value `val`.
  // NOTE: this assumes the entire set operation will occur within
  // one word in the bitfield. if this assumption becomes invalid, a new
  // multi-word set function should be created so we can still keep the
  // performance benefits of the assumption of this function.
  void SetBits(uint8_t idx, uint8_t size, uint8_t val) {
    storage_[idx / 8] &= ~(((1 << size) - 1) << (8 - (idx % 8) - size));
    storage_[idx / 8] |= val << (8 - (idx % 8) - size);
  }

  // get the entire byte containing the bit at index `idx`.
  // NOTE: assumes valid `idx`.
  uint8_t GetContainerByte(uint8_t idx) const { return storage_[idx / 8]; }

private:
  // zero-initialized word-aligned (for 8-bit AVR chip) storage array.
  uint8_t storage_[W] = {};
};
} // namespace util
} // namespace threeboard
