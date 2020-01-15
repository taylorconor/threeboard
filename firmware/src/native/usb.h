#pragma once

#include <stdint.h>

namespace threeboard {
namespace native {
class Usb {
public:
  virtual ~Usb() {}

  // Setup the USB connection to the host. This method blocks until setup.
  // TODO: return error handling status if necessary.
  virtual void Setup() = 0;

  virtual void SendKeypress(const uint8_t key, const uint8_t mod) = 0;
};
} // namespace native
} // namespace threeboard
