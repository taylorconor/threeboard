#pragma once

#include "src/native/native.h"
#include "src/usb/internal/hid_state.h"
#include "src/usb/usb.h"
#include "src/usb/usb_interrupt_handler_delegate.h"

namespace threeboard {
namespace usb {
class UsbImpl : public Usb, public UsbInterruptHandlerDelegate {
public:
  UsbImpl(native::Native *);
  void Setup() final override;
  void SendKeypress(const uint8_t key, const uint8_t mod) final override;

  void HandleGeneralInterrupt() final override;
  void HandleEndpointInterrupt() final override;

private:
  native::Native *native_;
  HidState hid_state_;

  int8_t SendKeypress();

  void AwaitTransmitterReady();
  void AwaitReceiverReady();
  void HandshakeTransmitterInterrupt();
  void HandshakeReceiverInterrupt();
  void SendHidState();
};
} // namespace usb
} // namespace threeboard
