#pragma once

#include "usb_protocol.h"
#include <avr/pgmspace.h>

namespace threeboard {
namespace native {

static const int16_t STR_MANUFACTURER[] = {L'B', L'a', L'n', L'a', L'n', L'a'};
static const int16_t STR_PRODUCT[] = {L'K', L'a', L'n', L'a', L'n'};

#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)

// Microchip vendor id. Terms of the license of this product id include only
// ever using it on a microcontroller designed by Microchip, e.g. the atmega32u4
// used for this project. microchip.com.
#define VENDOR_ID 0x04D8
// threeboard v1 product id.
#define PRODUCT_ID 0xEC51

#define ENDPOINT0_SIZE 32
#define KEYBOARD_INTERFACE 0
#define KEYBOARD_ENDPOINT 3
#define ENDPOINT_TYPE_IN 0x80
#define KEYBOARD_SIZE 8
#define EP_DOUBLE_BUFFER 0x06
#define EP_TYPE_INTERRUPT_IN 0xC1

// TODO: may not be necessary.
static const uint8_t PROGMEM endpoint_config_table[] = {
    0, 0, 1, EP_TYPE_INTERRUPT_IN, EP_DOUBLE_BUFFER, 0};

static const uint8_t PROGMEM device_descriptor[] = {
    18, // bLength
    1,  // bDescriptorType
    0x00,
    0x02,           // bcdUSB
    0,              // bDeviceClass
    0,              // bDeviceSubClass
    0,              // bDeviceProtocol
    ENDPOINT0_SIZE, // bMaxPacketSize0
    LSB(VENDOR_ID),
    MSB(VENDOR_ID), // idVendor
    LSB(PRODUCT_ID),
    MSB(PRODUCT_ID), // idProduct
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
    9,                      // bLength;
    2,                      // bDescriptorType;
    LSB(CONFIG1_DESC_SIZE), // wTotalLength
    MSB(CONFIG1_DESC_SIZE),
    1,    // bNumInterfaces
    1,    // bConfigurationValue
    0,    // iConfiguration
    0xC0, // bmAttributes
    50,   // bMaxPower
    // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
    9,                  // bLength
    4,                  // bDescriptorType
    KEYBOARD_INTERFACE, // bInterfaceNumber
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
    7,                                    // bLength
    5,                                    // bDescriptorType
    KEYBOARD_ENDPOINT | ENDPOINT_TYPE_IN, // bEndpointAddress
    0x03,                                 // bmAttributes (0x03=intr)
    KEYBOARD_SIZE, 0,                     // wMaxPacketSize
    1                                     // bInterval
};

struct UsbStringDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  const int16_t *wString;
};

// If you're desperate for a little extra code memory, these strings
// can be completely removed if iManufacturer, iProduct, iSerialNumber
// in the device desciptor are changed to zeros.
static const int16_t TEST[] = {0, 4, 0, 9};
static const UsbStringDescriptor PROGMEM string0 = {4, 3, TEST};
static const UsbStringDescriptor PROGMEM string1 = {sizeof(STR_MANUFACTURER), 3,
                                                    STR_MANUFACTURER};
static const UsbStringDescriptor PROGMEM string2 = {sizeof(STR_PRODUCT), 3,
                                                    STR_PRODUCT};

// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
static const DescriptorContainer PROGMEM descriptor_list[] = {
    {DescriptorValue(DescriptorType::DEVICE, 0), 0x0000, device_descriptor,
     sizeof(device_descriptor)},
    {DescriptorValue(DescriptorType::CONFIGURATION, 0), 0x0000,
     config1_descriptor, sizeof(config1_descriptor)},
    {DescriptorValue(DescriptorType::HID_REPORT, 0), KEYBOARD_INTERFACE,
     keyboard_hid_report_desc, sizeof(keyboard_hid_report_desc)},
    {DescriptorValue(DescriptorType::HID, 0), KEYBOARD_INTERFACE,
     config1_descriptor + KEYBOARD_HID_DESC_OFFSET, 9},
    {DescriptorValue(DescriptorType::STRING, 0), 0x0000,
     (const uint8_t *)&string0, 4},
    {DescriptorValue(DescriptorType::STRING, 1), 0x0409,
     (const uint8_t *)&string1, sizeof(STR_MANUFACTURER)},
    {DescriptorValue(DescriptorType::STRING, 2), 0x0409,
     (const uint8_t *)&string2, sizeof(STR_PRODUCT)}};

} // namespace native
} // namespace threeboard
