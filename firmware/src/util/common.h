#pragma once

#define __always_inline inline __attribute__((__always_inline__))

namespace util {
__always_inline constexpr uint8_t min(uint8_t a, uint8_t b) {
  return a < b ? a : b;
}

__always_inline constexpr uint8_t max(uint8_t a, uint8_t b) {
  return a < b ? b : a;
}
} // namespace util
