#pragma once

#include <stdint.h>

#define __force_inline inline __attribute__((__always_inline__))

#define CAT(A, B) A##B
#define SELECT(NAME, NUM) CAT(NAME##_, NUM)
#define GET_COUNT(_1, _2, _3, _4, _5, _6, COUNT, ...) COUNT
#define VA_SIZE(...) GET_COUNT(__VA_ARGS__, 6, 5, 4, 3, 2, 1)
#define VA_SELECT(NAME, ...) SELECT(NAME, VA_SIZE(__VA_ARGS__))(__VA_ARGS__)

#define RETURN_IF_ERROR(...) VA_SELECT(RETURN_IF_ERROR, __VA_ARGS__)

#define RETURN_IF_ERROR_1(x)       \
  if (!(x)) {                      \
    return false;                  \
  }                                \
  /* enforce trailing semicolon */ \
  static_assert(true, "")

#define RETURN_IF_ERROR_2(x, statement) \
  if (!(x)) {                           \
    statement;                          \
    return false;                       \
  }                                     \
  /* enforce trailing semicolon */      \
  static_assert(true, "")

#define WAIT_OR_RETURN(cond, max, err) \
  uint16_t iterations = 0;             \
  while ((cond)) {                     \
    iterations += 1;                   \
    if (iterations == (max)) {         \
      LOG((err));                      \
      return false;                    \
    }                                  \
  }                                    \
  /* enforce trailing semicolon */     \
  static_assert(true, "")

namespace threeboard {
namespace util {
__force_inline constexpr uint16_t min(uint16_t a, uint16_t b) {
  return a < b ? a : b;
}

__force_inline constexpr uint16_t max(uint16_t a, uint16_t b) {
  return a < b ? b : a;
}

__force_inline constexpr uint8_t lsb(const uint16_t &val) {
  return (uint8_t)val;
}

__force_inline constexpr uint8_t msb(const uint16_t &val) { return val >> 8; }

}  // namespace util
}  // namespace threeboard
