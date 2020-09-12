#pragma once

#include "src/native/native.h"
#include "src/usb/internal/hid_state.h"
#include "src/usb/shared/constants.h"
#include "src/usb/shared/protocol.h"

namespace threeboard {
namespace usb {

class RequestHandler {
public:
  RequestHandler(native::Native *native);
  //  virtual ~RequestHandler() {}

  // Device handlers.
  virtual void HandleGetStatus();
  virtual void HandleSetAddress(const SetupPacket &);
  virtual void HandleGetDescriptor(const SetupPacket &);
  virtual void HandleGetConfiguration(const HidState &);
  virtual void HandleSetConfiguration(const SetupPacket &, HidState *);

  // HID handlers.
  virtual void HandleGetReport(const HidState &);
  virtual void HandleGetIdle(const HidState &);
  virtual void HandleSetIdle(const SetupPacket &, HidState *);
  virtual void HandleGetProtocol(const HidState &);
  virtual void HandleSetProtocol(const SetupPacket &, HidState *);

private:
  native::Native *native_;
};
} // namespace usb
} // namespace threeboard
