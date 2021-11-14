#pragma once

#include "src/delegates/usb_interrupt_handler_delegate.h"
#include "src/native/native.h"
#include "src/usb/internal/hid_state.h"
#include "src/usb/internal/request_handler.h"
#include "src/usb/usb_controller.h"

namespace threeboard {
namespace usb {

// This USB implementation is influenced by the LUFA project
// (https://github.com/abcminiuser/lufa), and by the Atreus firmware
// (https://github.com/technomancy/atreus-firmware).
// It explicitly does not support the ENDPOINT_HALT feature, since it's rarely
// used and shouldn't affect functionality at all.
class UsbControllerImpl final : public UsbController,
                                public UsbInterruptHandlerDelegate {
 public:
  UsbControllerImpl(native::Native *);

  bool Setup() override;
  bool HasConfigured() override;
  bool SendKeypress(uint8_t key, uint8_t mod) override;

  void HandleGeneralInterrupt() override;
  void HandleEndpointInterrupt() override;

 private:
  friend class UsbImplTest;

  bool SendKeypress();
  void SendHidState();

  native::Native *native_;
  HidState hid_state_;
  RequestHandler *request_handler_;
};
}  // namespace usb
}  // namespace threeboard
