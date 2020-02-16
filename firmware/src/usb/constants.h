#pragma once

#include <stdint.h>

namespace threeboard {
namespace usb {

// Microchip vendor id. Terms of the license of this product id include only
// ever using it on a microcontroller designed by Microchip, e.g. the atmega32u4
// used for this project. microchip.com.
constexpr uint16_t kVendorId = 0x04D8;
// threeboard v1 product id.
constexpr uint16_t kProductId = 0xEC51;

namespace hid {
constexpr uint16_t kSpecificationComplianceVersion = 0x1011; // HID spec v1.11.
constexpr uint8_t kNumInterfaces = 1;
constexpr uint8_t kNotLocalized = 0;
constexpr uint8_t kHidClassCode = 3;
constexpr uint8_t kBootInterfaceSubclassCode = 1;
constexpr uint8_t kKeyboardInterfaceProtocol = 1;
} // namespace hid

constexpr uint16_t kUsbSpecificationReleaseNumber = 0x0200; // USB spec 2.0.
constexpr uint16_t kLanguageIdEnglish = 0x0409;
constexpr uint8_t kDeviceClassCode = 0;
constexpr uint8_t kDeviceSubclassCode = 0;
constexpr uint8_t kFullSpeedDeviceProtocol = 0;
constexpr uint8_t kKeyboardEndpoint = 1;
constexpr uint8_t kKeyboardEndpointTransferType = 3;
constexpr uint8_t kKeyboardConfigurationValue = 1;
constexpr uint8_t kKeyboardInterface = 0;
constexpr uint8_t kKeyboardMaxPacketSize = 8;
constexpr uint8_t kConfigurationAttributeSelfPowered = 0b11000000;
constexpr uint8_t kEndpointTypeInterrupt = 0b11000000;
constexpr uint8_t kEndpointPipeTypeIn = 0b10000000;
constexpr uint8_t kEndpointDirectionIn = 0b00000001;
constexpr uint8_t kEndpoint32ByteBank = 0b00100000;
constexpr uint8_t kEndpointSingleBank = 0b00000010;
constexpr uint8_t kEndpointDoubleBank = 0b00000110;

} // namespace usb
} // namespace threeboard
