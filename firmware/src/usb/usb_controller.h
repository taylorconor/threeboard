#pragma once

#include <stdint.h>

namespace threeboard {
namespace usb {

class UsbController {
 public:
  virtual ~UsbController() = default;

  // Setup the USB hardware to begin connection to the host. This method blocks
  // until the device hardware is correctly set up. If this method returns false
  // it means setup was halted by an error issued via the ErrorHandlerDelegate
  // and is in an undefined state.
  virtual bool Setup() = 0;

  // Returns true if the HID device configuration has completed successfully.
  virtual bool HasConfigured() = 0;

  // Send the provided key and modifier code to the host device. Returns false
  // if an error occurred during sending.
  virtual bool SendKeypress(uint8_t key, uint8_t mod) = 0;
};
}  // namespace usb
}  // namespace threeboard
