#pragma once

#include "usb_common.h"
#include "usb_protocol.h"

namespace threeboard {
namespace native {
namespace device_handler {

void HandleGetStatus();
void HandleSetAddress(const SetupPacket &packet);
void HandleGetDescriptor(const SetupPacket &packet);
void HandleGetConfiguration(const UsbHidState &hid_state);
void HandleSetConfiguration(const SetupPacket &packet, UsbHidState *hid_state);
} // namespace device_handler

namespace hid_handler {

void HandleGetReport(const UsbHidState &hid_state);
void HandleGetIdle(const UsbHidState &hid_state);
void HandleSetIdle(const SetupPacket &packet, UsbHidState *hid_state);
void HandleGetProtocol(const UsbHidState &hid_state);
void HandleSetProtocol(const SetupPacket &packet, UsbHidState *hid_state);
} // namespace hid_handler
} // namespace native
} // namespace threeboard
