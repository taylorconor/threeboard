#pragma once

#include "usb_protocol.h"

namespace threeboard {
namespace native {
namespace device_handler {

void HandleGetStatus();
void HandleSetAddress(const SetupPacket &packet);
void HandleGetDescriptor(const SetupPacket &packet);
void HandleGetConfiguration(const volatile uint8_t &usb_configuration);
void HandleSetConfiguration(const SetupPacket &packet,
                            volatile uint8_t *usb_configuration);
} // namespace device_handler
} // namespace native
} // namespace threeboard
