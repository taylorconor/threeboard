#include "enum.h"

#include <gtest/gtest.h>

namespace threeboard {
namespace util {
namespace {

TEST(EnumTest, CalculatesCorrectEnumLength) {
  enum TestEnum : uint8_t { ONE, TWO, THREE, FOUR, FIVE };
  ASSERT_EQ(element_count<TestEnum>(), 5);
}

TEST(EnumTest, CalculatesCorrectEnumLengthForExplicitInitializers) {
  enum TestEnum : uint8_t {
    TWO = 2,
    FOUR = 4,
    SIX = 6,
    EIGHT = 8,
    TEN = 10,
    TWELVE = 12
  };
  ASSERT_EQ(element_count<TestEnum>(), 6);
}

TEST(EnumTest, NoCountForEmptyEnums) {
  enum TestEnum : uint8_t {};
  ASSERT_EQ(element_count<TestEnum>(), 0);
}
} // namespace
} // namespace util
} // namespace threeboard
