#pragma once

#include "../util/common.h"
#include "usb_common.h"
#include "usb_protocol.h"
#include <avr/pgmspace.h>

namespace threeboard {
namespace native {

// A container for a generic descriptor. Used in a list as a descriptor lookup
// table.
struct DescriptorContainer {
  DescriptorId id;
  uint16_t index;
  const uint8_t *data;
  uint8_t length;

  // TODO: this can probably be made constexpr with some adjustments, if needed.
  // https://www.avrfreaks.net/forum/better-alternative-pgmreadword-etc.
  static DescriptorContainer ParseFromProgmem(const uint8_t *ptr);
};

// The top-level USB device descriptor.
static constexpr DeviceDescriptor PROGMEM device_descriptor = {
    .bcdUSB = kUsbSpecificationReleaseNumber,
    .bDeviceClass = kDeviceClassCode,
    .bDeviceSubClass = kDeviceSubclassCode,
    .bDeviceProtocol = kFullSpeedDeviceProtocol,
    .bMaxPacketSize0 = kEndpoint32ByteBank,
    .idVendor = kVendorId,
    .idProduct = kProductId,
    .bcdDevice = 1,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 0,
    .bNumConfigurations = 1};

// The HID report object for the keyboard. The format of this object is quite
// nasty and obscure, and I think it would be even more confusing if it were
// strictly typed. It's a mostly verbatim subset of the example from the HID
// spec v1.11, section E.6.
static constexpr uint8_t PROGMEM hid_report[] = {
    0x05, 0x01, // Usage Page (Generic Desktop),
    0x09, 0x06, // Usage (Keyboard),
    0xA1, 0x01, // Collection (Application),
    0x75, 0x01, //   Report Size (1),
    0x95, 0x08, //   Report Count (8),
    0x05, 0x07, //   Usage Page (Key Codes),
    0x19, 0xE0, //   Usage Minimum (224),
    0x29, 0xE7, //   Usage Maximum (231),
    0x15, 0x00, //   Logical Minimum (0),
    0x25, 0x01, //   Logical Maximum (1),
    0x81, 0x02, //   Input (Data, Variable, Absolute), ;Modifier byte
    0x95, 0x06, //   Report Count (6),
    0x75, 0x08, //   Report Size (8),
    0x15, 0x00, //   Logical Minimum (0),
    0x25, 0xff, //   Logical Maximum(255),
    0x05, 0x07, //   Usage Page (Key Codes),
    0x19, 0x00, //   Usage Minimum (0),
    0x29, 0xff, //   Usage Maximum (255),
    0x81, 0x00, //   Input (Data, Array),
    0xc0        // End Collection
};

/*static constexpr uint8_t PROGMEM hid_report[] = {
    0x05, 0x01, 0x09, 0x06, 0xa1, 0x01, 0x05, 0x07, 0x19, 0xe0, 0x29,
    0xe7, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x08, 0x81, 0x02,
    0x95, 0x01, 0x75, 0x08, 0x81, 0x03, 0x95, 0x03, 0x75, 0x01, 0x05,
    0x08, 0x19, 0x01, 0x29, 0x03, 0x91, 0x02, 0x95, 0x01, 0x75, 0x05,
    0x91, 0x03, 0x95, 0x06, 0x75, 0x08, 0x15, 0x00,0x26, 0xa4, 0x00, 0x05, 0x07,
   0x19, 0x00, 0x29, 0xa4, 0x81, 0x00, 0xc0
    };*/

struct CombinedDescriptor {
  ConfigurationDescriptor configuration_descriptor;
  InterfaceDescriptor interface_descriptor;
  HidDescriptor hid_descriptor;
  EndpointDescriptor endpoint_descriptor;
};

// The CombinedDescriptor is a collection of all additional (i.e. non-device)
// USB descriptors needed for this USB implementation. It's defined like this to
// force the compiler to allocate a contiguous piece of program memory to store
// the entire combined descrior in.
static constexpr CombinedDescriptor PROGMEM combined_descriptor = {
    .configuration_descriptor =
        {
            .wTotalLength = sizeof(CombinedDescriptor),
            .bNumInterfaces = hid::kNumInterfaces,
            .bConfigurationValue = kKeyboardConfigurationValue,
            .iConfiguration = 0,
            .bmAttributes = kConfigurationAttributeSelfPowered,
            .bMaxPower = 50,
        },
    .interface_descriptor =
        {
            .bInterfaceNumber = kKeyboardInterface,
            .bAlternateSetting = 0,
            .bNumEndpoints = 1,
            .bInterfaceClass = hid::kHidClassCode,
            .bInterfaceSubClass = hid::kBootInterfaceSubclassCode,
            .bInterfaceProtocol = hid::kKeyboardInterfaceProtocol,
            .iInterface = 0,
        },
    .hid_descriptor =
        {
            .bcdHID = hid::kSpecificationComplianceVersion,
            .bCountryCode = hid::kNotLocalized,
            .bNumDescriptors = 1,
            .bReportDescriptorType = DescriptorType::HID_REPORT,
            .wReportDescriptorLength = sizeof(hid_report),
        },
    .endpoint_descriptor = {.bEndpointAddress =
                                kKeyboardEndpoint | kEndpointPipeTypeIn,
                            .bmAttributes = kKeyboardEndpointTransferType,
                            .wMaxPacketSize = kKeyboardMaxPacketSize,
                            .bInterval = 1}};

// Define all of the string descriptors we send from this device. The
// supported_languages decriptor is mandatory, the others are just so the host
// receives a human-redable device identifier string, and can be removed if
// necessary.
static constexpr UnicodeStringDescriptor PROGMEM supported_languages = {
    .bLength = 4, .bString = {kLanguageIdEnglish}};
static constexpr UnicodeStringDescriptor PROGMEM manufacturer = {
    .bLength = 30,
    .bDescriptorType = DescriptorType::STRING,
    {.bString = L"threeboard.dev"}};
static constexpr UnicodeStringDescriptor PROGMEM product = {
    .bLength = 32,
    .bDescriptorType = DescriptorType::STRING,
    {.bString = L"threeboard v0.1"}};

// This table defines which descriptor data is sent for each specific
// GetDescriptor request from the host, for a given DescriptorValue.
// This can't be constexpr because of the implicit reinterpret_cast operations
// used to convert the descriptors into uint8_t* values for transmission.
static const DescriptorContainer PROGMEM descriptor_list[] = {
    {.id = DescriptorType::DEVICE,
     .index = 0,
     .data = (uint8_t *)&device_descriptor,
     .length = sizeof(device_descriptor)},
    {.id = DescriptorType::CONFIGURATION,
     .index = 0,
     .data = (uint8_t *)&combined_descriptor,
     .length = sizeof(combined_descriptor)},
    {.id = DescriptorType::HID_REPORT,
     .index = kKeyboardInterface,
     .data = hid_report,
     .length = sizeof(hid_report)},
    {.id = DescriptorType::HID,
     .index = kKeyboardInterface,
     // TODO: i'm 99% sure this is UB. But it works.
     .data = (uint8_t *)(void *)&combined_descriptor.hid_descriptor,
     .length = combined_descriptor.hid_descriptor.bLength},
    {.id = {DescriptorType::STRING, 0},
     .index = 0,
     .data = (uint8_t *)&supported_languages,
     .length = supported_languages.bLength},
    {.id = {DescriptorType::STRING, 1},
     .index = kLanguageIdEnglish,
     .data = (uint8_t *)&manufacturer,
     .length = manufacturer.bLength},
    {.id = {DescriptorType::STRING, 2},
     .index = kLanguageIdEnglish,
     .data = (uint8_t *)&product,
     .length = product.bLength}};

} // namespace native
} // namespace threeboard
