#pragma once

#include "src/usb/internal/constants.h"
#include "src/usb/internal/hid_state.h"
#include "src/usb/internal/protocol.h"

namespace threeboard {
namespace usb {
namespace device_handler {

void HandleGetStatus(native::Native *);
void HandleSetAddress(native::Native *, const SetupPacket &);
void HandleGetDescriptor(native::Native *, const SetupPacket &);
void HandleGetConfiguration(native::Native *, const HidState &);
void HandleSetConfiguration(native::Native *, const SetupPacket &, HidState *);
} // namespace device_handler

namespace hid_handler {

void HandleGetReport(native::Native *, const HidState &);
void HandleGetIdle(native::Native *, const HidState &);
void HandleSetIdle(native::Native *, const SetupPacket &, HidState *);
void HandleGetProtocol(native::Native *, const HidState &);
void HandleSetProtocol(native::Native *, const SetupPacket &, HidState *);
} // namespace hid_handler
} // namespace usb
} // namespace threeboard
