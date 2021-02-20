#pragma once

#include <stdint.h>

namespace threeboard {

// An identifier for each layer in the threeboard.
enum LayerId : uint8_t {
  DFLT = 0,
  R = 1,
  G = 2,
  B = 3,
};

}  // namespace threeboard
