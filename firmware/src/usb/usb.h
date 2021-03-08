#pragma once

#include <stdint.h>

namespace threeboard {
namespace usb {

class Usb {
 public:
  virtual ~Usb() = default;

  // Setup the USB hardware to begin connection to the host. This method blocks
  // until the device hardware is correctly setup. This method returns true on
  // success. If this method returns false it means setup was halted by an error
  // issued via the ErrorHandlerDelegate and is in an undefined state.
  virtual bool Setup() = 0;
  virtual bool HasConfigured() = 0;

  virtual void SendKeypress(uint8_t key, uint8_t mod) = 0;
};
}  // namespace usb
}  // namespace threeboard
