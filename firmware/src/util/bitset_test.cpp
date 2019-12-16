#include "bitset.h"

#include <gtest/gtest.h>

namespace threeboard {
namespace util {
namespace {

TEST(BitsetTest, IsCorrectSize) {
  bitset<7> bitset7;
  ASSERT_EQ(sizeof(uint8_t), sizeof(bitset7));
  bitset<8> bitset8;
  ASSERT_EQ(sizeof(uint8_t) * 2, sizeof(bitset8));
  bitset<88> bitset88;
  ASSERT_EQ(sizeof(uint8_t) * 12, sizeof(bitset88));
}

TEST(BitsetTest, SetBit) {
  bitset<15> bitset;
  // Set some bits in the first byte and in the second byte.
  bitset.SetBit(0, 1);
  bitset.SetBit(2, 1);
  bitset.SetBit(14, 1);
  bitset.SetBit(15, 1);
  // Verify correct bits are set in each byte.
  ASSERT_EQ(bitset.GetContainerByte(0), 0b00000101);
  ASSERT_EQ(bitset.GetContainerByte(8), 0b11000000);
}

TEST(BitsetTest, SetBits) {
  bitset<15> bitset;
  // Set first 3 bits of byte 0 to 1.
  bitset.SetBits(0, 3, 0b00000111);
  // Set last 6 bits of byte 1 to 1.
  bitset.SetBits(10, 6, 0b00111111);
  // Verify correct bits are set in each byte.
  ASSERT_EQ(bitset.GetContainerByte(0), 0b11100000);
  ASSERT_EQ(bitset.GetContainerByte(8), 0b00111111);
}

TEST(BitsetTest, UnsetBit) {
  bitset<15> bitset;
  // Set entire bitset to 1s.
  bitset.SetBits(0, 8, 0b11111111);
  bitset.SetBits(8, 8, 0b11111111);
  // Unset some bits in the first byte and in the second byte.
  bitset.SetBit(0, 0);
  bitset.SetBit(2, 0);
  bitset.SetBit(14, 0);
  bitset.SetBit(15, 0);
  // Verify bits have been unset
  ASSERT_EQ(bitset.GetContainerByte(0), 0b11111010);
  ASSERT_EQ(bitset.GetContainerByte(8), 0b00111111);
}

TEST(BitsetTest, SetInvalidBitInSameByteOk) {
  // A 1-bit bitset still allocates a full byte, and doesn't do any runtime
  // bounds checking.
  bitset<1> bitset;
  bitset.SetBit(7, 1);
  ASSERT_EQ(bitset.GetContainerByte(0), 0b10000000);
}
} // namespace
} // namespace util
} // namespace threeboard
