#pragma once

#include "src/delegates/error_handler_delegate.h"
#include "src/delegates/usb_interrupt_handler_delegate.h"
#include "src/native/native.h"
#include "src/usb/internal/hid_state.h"
#include "src/usb/internal/request_handler.h"
#include "src/usb/usb.h"

namespace threeboard {
namespace usb {

// This USB implementation is influenced by the LUFA project
// (https://github.com/abcminiuser/lufa), and by the Atreus firmware
// (https://github.com/technomancy/atreus-firmware).
// It explicitly does not support the ENDPOINT_HALT feature, since it's rarely
// used and shouldn't affect functionality at all.
class UsbImpl : public Usb, public UsbInterruptHandlerDelegate {
 public:
  UsbImpl(native::Native *, ErrorHandlerDelegate *);

  bool Setup() final;
  bool HasConfigured() final;
  void SendKeypress(uint8_t key, uint8_t mod) final;

  void HandleGeneralInterrupt() final;
  void HandleEndpointInterrupt() final;

 private:
  friend class UsbImplRequestHandlingTest;

  native::Native *native_;
  ErrorHandlerDelegate *error_handler_;
  HidState hid_state_;
  RequestHandler *request_handler_;

  int8_t SendKeypress();
  void SendHidState();
};
}  // namespace usb
}  // namespace threeboard
