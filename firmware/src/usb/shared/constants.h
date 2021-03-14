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

// Specify endpoint type as "interrupt" in UECFG0X.
constexpr uint8_t kEndpointTypeInterrupt = 0b11000000;

// Used to set EPDIR (endpoint direction) in UECFG0X as IN (device to host).
constexpr uint8_t kEndpointDirectionIn = 0b00000001;

// Set to allocate endpoint memory. Unset to clear endpoint memory.
constexpr uint8_t kEndpointAlloc = 0b00000010;

// Set the endpoint size to double bank (default is single bank).
constexpr uint8_t kEndpointDoubleBank = 0b00000100;

// Maximum packet size.
constexpr uint8_t k32BytePacketSize = 0b00100000;

// USB HID-related constants.
namespace hid {

// Identifier for the HID spec v1.11.
constexpr uint16_t kSpecificationComplianceVersion = 0x1011;

// Amount of interfaces used in the ConfigurationDescriptor.
constexpr uint8_t kNumInterfaces = 1;

// HID country code: no specific country.
constexpr uint8_t kNotLocalized = 0;

// USB class code for HID (Human Interface Device). HID spec v1.11, section 4.1.
constexpr uint8_t kHidClassCode = 3;

// HID boot interface subclass. HID spec v1.11, section 4.2.
constexpr uint8_t kBootInterfaceSubclassCode = 1;

// USB keyboard protocol code. HID spec v1.11, section 4.3.
constexpr uint8_t kKeyboardInterfaceProtocol = 1;
}  // namespace hid

// USB Descriptor-related constants.
namespace descriptor {

// Specify the language ID for string descriptors.
constexpr uint16_t kLanguageIdEnglish = 0x0409;

// Specify USB Release Number 2.0 in Binary-Coded Decimal. USB spec v2.0,
// section 9.6.1.
constexpr uint16_t kUsbSpecificationReleaseNumber = 0x0200;

// USB base device class code. Indicates to use class code info from Interface
// Descriptors. Subclass and protocol are unused.
constexpr uint8_t kBaseDeviceClassCode = 0;
constexpr uint8_t kBaseDeviceSubclassCode = 0;
constexpr uint8_t kBaseDeviceProtocol = 0;

// Specify the ID of the endpoint to use for the HID keyboard protocol.
constexpr uint8_t kKeyboardEndpoint = 1;

// Configure the keyboard endpoint to use the 'interrupt' transfer type. USB
// spec v2.0, section 9.6.9, table 9-13.
constexpr uint8_t kKeyboardEndpointAttributes = 0b00000011;

// Zero-based value identifying the index in the array of concurrent interfaces
// supported by this configuration. Only one configuration is supported by the
// threeboard.
constexpr uint8_t kKeyboardInterfaceIndex = 0;

// Maximum packet size the keyboard endpoint is capable of sending or receiving
// when this configuration is selected.
constexpr uint8_t kKeyboardEndpointMaxPacketSize = 8;

// Attributes used in the configuration descriptor. Only bit 7 (reserved) is set
// as it must be set to 1.
constexpr uint8_t kConfigurationAttributes = 0b10000000;

// Max bus power specified in the configuration descriptor. It's measured in
// increments of 2mA, so this indicates we use 100mA of bus power.
constexpr uint8_t kConfigurationMaxPower = 50;

// Used in bEndpointAddress to specify endpoint direction as IN (device to
// host).
constexpr uint8_t kEndpointPipeTypeIn = 0b10000000;

// Value to use as an argument to the SetConfiguration request to select the
// specified configuration. The threeboard only supports one configuration, so
// this is just used to verify that the host isn't trying to put the threeboard
// into an unsupported configuration.
constexpr uint8_t kConfigurationValue = 1;

}  // namespace descriptor
}  // namespace usb
}  // namespace threeboard
