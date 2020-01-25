#pragma once

#include "../util/common.h"
#include "usb_protocol.h"
#include <avr/pgmspace.h>

namespace threeboard {
namespace native {

//#define EP_DOUBLE_BUFFER 0x06
//#define EP_TYPE_INTERRUPT_IN 0xC1

// TODO: may not be necessary.
// static const uint8_t PROGMEM endpoint_config_table[] = {
//    0, 0, 1, EP_TYPE_INTERRUPT_IN, EP_DOUBLE_BUFFER, 0};

static const uint8_t PROGMEM device_descriptor[] = {
    18, // bLength
    1,  // bDescriptorType
    0x00,
    0x02,                // bcdUSB
    0,                   // bDeviceClass
    0,                   // bDeviceSubClass
    0,                   // bDeviceProtocol
    kEndpoint32ByteBank, // bMaxPacketSize0
    util::lsb(kVendorId),
    util::msb(kVendorId), // idVendor
    util::lsb(kProductId),
    util::msb(kProductId), // idProduct
    0x00,
    0x01, // bcdDevice
    1,    // iManufacturer
    2,    // iProduct
    0,    // iSerialNumber
    1     // bNumConfigurations
};

// Keyboard Protocol 1, HID 1.11 spec, Appendix B, page 59-60
static const uint8_t PROGMEM keyboard_hid_report_desc[] = {
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
    0x95, 0x01, //   Report Count (1),
    0x75, 0x08, //   Report Size (8),
    0x81, 0x03, //   Input (Constant),                 ;Reserved byte
    0x95, 0x05, //   Report Count (5),
    0x75, 0x01, //   Report Size (1),
    0x05, 0x08, //   Usage Page (LEDs),
    0x19, 0x01, //   Usage Minimum (1),
    0x29, 0x05, //   Usage Maximum (5),
    0x91, 0x02, //   Output (Data, Variable, Absolute), ;LED report
    0x95, 0x01, //   Report Count (1),
    0x75, 0x03, //   Report Size (3),
    0x91, 0x03, //   Output (Constant),                 ;LED report padding
    0x95, 0x06, //   Report Count (6),
    0x75, 0x08, //   Report Size (8),
    0x15, 0x00, //   Logical Minimum (0),
    0x25, 0x68, //   Logical Maximum(104),
    0x05, 0x07, //   Usage Page (Key Codes),
    0x19, 0x00, //   Usage Minimum (0),
    0x29, 0x68, //   Usage Maximum (104),
    0x81, 0x00, //   Input (Data, Array),
    0xc0        // End Collection
};

#define CONFIG1_DESC_SIZE (9 + 9 + 9 + 7)
#define KEYBOARD_HID_DESC_OFFSET (9 + 9)
static const uint8_t PROGMEM config1_descriptor[CONFIG1_DESC_SIZE] = {
    // configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
    9,                                      // bLength;
    (uint8_t)DescriptorType::CONFIGURATION, // bDescriptorType;
    util::lsb(CONFIG1_DESC_SIZE),           // wTotalLength
    util::msb(CONFIG1_DESC_SIZE),
    hid::kNumInterfaces, // bNumInterfaces
    1,                   // bConfigurationValue
    0,                   // iConfiguration
    0xC0,                // bmAttributes
    50,                  // bMaxPower
    // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
    9,                  // bLength
    4,                  // bDescriptorType
    kKeyboardInterface, // bInterfaceNumber
    0,                  // bAlternateSetting
    1,                  // bNumEndpoints
    0x03,               // bInterfaceClass (0x03 = HID)
    0x01,               // bInterfaceSubClass (0x01 = Boot)
    0x01,               // bInterfaceProtocol (0x01 = Keyboard)
    0,                  // iInterface
    // HID interface descriptor, HID 1.11 spec, section 6.2.1
    9,                                // bLength
    0x21,                             // bDescriptorType
    0x11, 0x01,                       // bcdHID
    0,                                // bCountryCode
    1,                                // bNumDescriptors
    0x22,                             // bDescriptorType
    sizeof(keyboard_hid_report_desc), // wDescriptorLength
    0,
    // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
    7,                                   // bLength
    5,                                   // bDescriptorType
    kKeyboardEndpoint | kEndpointTypeIn, // bEndpointAddress
    0x03,                                // bmAttributes (0x03=intr)
    kKeyboardMaxPacketSize, 0,           // wMaxPacketSize
    1                                    // bInterval
};

struct UsbStringDescriptor {
  uint8_t length;
  DescriptorType descriptor_type;
  const wchar_t string[];
};

static const UsbStringDescriptor PROGMEM supported_languages = {
    4, DescriptorType::STRING, {kLanguageIdEnglish}};
static const UsbStringDescriptor PROGMEM manufacturer = {
    30, DescriptorType::STRING, L"threeboard.dev"};
static const UsbStringDescriptor PROGMEM product = {32, DescriptorType::STRING,
                                                    L"threeboard v0.1"};

// This table defines which descriptor data is sent for each specific
// GetDescriptor request from the host, for a given DescriptorValue.
static const DescriptorContainer PROGMEM descriptor_list[] = {
    {{DescriptorType::DEVICE, 0},
     0,
     device_descriptor,
     sizeof(device_descriptor)},
    {{DescriptorType::CONFIGURATION, 0},
     0,
     config1_descriptor,
     sizeof(config1_descriptor)},
    {{DescriptorType::HID_REPORT, 0},
     kKeyboardInterface,
     keyboard_hid_report_desc,
     sizeof(keyboard_hid_report_desc)},
    {{DescriptorType::HID, 0},
     kKeyboardInterface,
     config1_descriptor + KEYBOARD_HID_DESC_OFFSET,
     9},
    {{DescriptorType::STRING, 0},
     0,
     (const uint8_t *)&supported_languages,
     supported_languages.length},
    {{DescriptorType::STRING, 1},
     kLanguageIdEnglish,
     (const uint8_t *)&manufacturer,
     manufacturer.length},
    {{DescriptorType::STRING, 2},
     kLanguageIdEnglish,
     (const uint8_t *)&product,
     product.length}};

} // namespace native
} // namespace threeboard
