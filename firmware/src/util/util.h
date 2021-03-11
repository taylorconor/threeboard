#pragma once

#include <stdint.h>

#define __force_inline inline __attribute__((__always_inline__))

#define RETURN_IF_ERROR(x) \
  if (!(x)) {              \
    return false;          \
  }

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
