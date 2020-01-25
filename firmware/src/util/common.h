#pragma once

#include <stdint.h>

#define __always_inline inline __attribute__((__always_inline__))

namespace threeboard {
namespace util {
__always_inline constexpr uint8_t min(uint8_t a, uint8_t b) {
  return a < b ? a : b;
}

__always_inline constexpr uint8_t max(uint8_t a, uint8_t b) {
  return a < b ? b : a;
}

__always_inline constexpr uint8_t lsb(const uint16_t &val) {
  return (uint8_t)val;
}

__always_inline constexpr uint8_t msb(const uint16_t &val) { return val >> 8; }
} // namespace util
} // namespace threeboard
