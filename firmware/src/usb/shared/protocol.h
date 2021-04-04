#pragma once

#include <stdint.h>

#include "src/native/native.h"
#include "src/util/util.h"

namespace threeboard {
namespace usb {

// RequestType bitmap format as defined by the USB spec rev. 2.0, section 9.3,
// table 9-2.
class RequestType {
 public:
  constexpr RequestType() {}

  enum class Direction : uint8_t {
    HOST_TO_DEVICE = 0,
    DEVICE_TO_HOST = 1,
  };
  enum class Type : uint8_t {
    STANDARD = 0,
    CLASS = 1,
    VENDOR = 2,
    RESERVED = 3,
  };
  enum class Recipient : uint8_t {
    DEVICE = 0,
    INTERFACE = 1,
    ENDPOINT = 2,
    OTHER = 3,
  };

  constexpr RequestType(const uint8_t value) : value_(value) {}
  constexpr RequestType(const Direction direction,
                        const Type type = Type::STANDARD,
                        const Recipient recipient = Recipient::DEVICE)
      : value_((uint8_t)recipient | ((uint8_t)type << 5) |
               ((uint8_t)direction << 7)) {}

  __force_inline Direction GetDirection() const {
    return (Direction)(value_ >> 7);
  }
  __force_inline Type GetType() const { return (Type)((value_ >> 5) & 3); }
  __force_inline Recipient GetRecipient() const {
    return (Recipient)(value_ & 31);
  }
  __force_inline uint8_t GetValue() const { return value_; }

 private:
  uint8_t value_ = 0;
};

// USB request codes as defined by the USB spec rev. 2.0, section 9.3, table
// 9-4. Class-specific HID request codes as defined by the USB HID firmware
// specification v1.11, section 7.2. This device will only ever be an HID class
// device.
enum class Request : uint8_t {
  GET_STATUS = 0x00,
  CLEAR_FEATURE = 0x01,
  SET_FEATURE = 0x03,
  SET_ADDRESS = 0x05,
  GET_DESCRIPTOR = 0x06,
  SET_DESCRIPTOR = 0x07,
  GET_CONFIGURATION = 0x08,
  SET_CONFIGURATION = 0x09,
  GET_INTERFACE = 0x0A,
  SET_INTERFACE = 0x0B,
  SYNCH_FRAME = 0x0C,

  HID_GET_REPORT = 0x01,
  HID_GET_IDLE = 0x02,
  HID_GET_PROTOCOL = 0x03,
  HID_SET_REPORT = 0x09,
  HID_SET_IDLE = 0x0A,
  HID_SET_PROTOCOL = 0x0B,
};

// Descriptor types for GET_DESCRIPTOR as defined by the USB spec rev. 2.0,
// section 9.4, table 9-5. Class-specific HID descriptor types as defined by the
// USB HID firmware specification v1.11, section 7.1.
enum class DescriptorType : uint8_t {
  DEVICE = 0x01,
  CONFIGURATION = 0x02,
  STRING = 0x03,
  INTERFACE = 0x04,
  ENDPOINT = 0x05,
  DEVICE_QUALIFIER = 0x06,
  OTHER_SPEED_CONFIGURATION = 0x07,
  INTERFACE_POWER = 0x08,

  HID = 0x21,
  HID_REPORT = 0x22,
  HID_PHYSICAL_DESCRIPTOR = 0x23,
};

// Global descriptor identifier, derived from the value field of the
// descriptor, stored in wValue. Defined by the USB spec rev. 2.0,
// section 9.4.3.
class DescriptorId {
 public:
  // We need explicit default constructor here to allow Descriptor to remain an
  // aggregate, but it serves no other purpose.
  constexpr DescriptorId() = default;

  // Descriptor type in the high byte, descriptor index in the low byte.
  constexpr DescriptorId(const DescriptorType &type, uint8_t index)
      : value_(((uint8_t)type << 8) | index) {}

  constexpr DescriptorId(const DescriptorType &type)
      : value_((uint8_t)type << 8) {}

  // Also allow implicit initialization from a regular uint16_t.
  constexpr DescriptorId(const uint16_t &value) : value_(value) {}

  constexpr bool operator==(const DescriptorId &other) const {
    return this->value_ == other.value_;
  }

  constexpr uint16_t GetValue() const { return value_; }

 private:
  uint16_t value_ = 0;
};

// USB spec rev. 2.0, section 9.6.1, table 9-8.
struct DeviceDescriptor {
  uint8_t bLength;
  DescriptorType bDescriptorType;
  uint16_t bcdUSB;
  uint8_t bDeviceClass;
  uint8_t bDeviceSubClass;
  uint8_t bDeviceProtocol;
  uint8_t bMaxPacketSize0;
  uint16_t idVendor;
  uint16_t idProduct;
  uint16_t bcdDevice;
  uint8_t iManufacturer;
  uint8_t iProduct;
  uint8_t iSerialNumber;
  uint8_t bNumConfigurations;
};

// USB spec rev. 2.0, section 9.6.3, table 9-10.
struct ConfigurationDescriptor {
  uint8_t bLength;
  DescriptorType bDescriptorType;
  uint16_t wTotalLength;
  uint8_t bNumInterfaces;
  uint8_t bConfigurationValue;
  uint8_t iConfiguration;
  uint8_t bmAttributes;
  uint8_t bMaxPower;
};

// USB spec rev. 2.0, section 9.6.5, table 9-12.
struct InterfaceDescriptor {
  uint8_t bLength;
  DescriptorType bDescriptorType;
  uint8_t bInterfaceNumber;
  uint8_t bAlternateSetting;
  uint8_t bNumEndpoints;
  uint8_t bInterfaceClass;
  uint8_t bInterfaceSubClass;
  uint8_t bInterfaceProtocol;
  uint8_t iInterface;
};

// HID spec v1.11, section 6.2.1.
struct HidDescriptor {
  uint8_t bLength;
  DescriptorType bDescriptorType;
  uint16_t bcdHID;
  uint8_t bCountryCode;
  uint8_t bNumDescriptors;
  DescriptorType bReportDescriptorType;
  uint16_t wReportDescriptorLength;
};

// USB spec rev. 2.0, section 9.6.6, table 9-13.
struct EndpointDescriptor {
  uint8_t bLength;
  DescriptorType bDescriptorType;
  uint8_t bEndpointAddress;
  uint8_t bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t bInterval;
};

// USB spec rev. 2.0, section 9.6.7, table 9-16.
template <int T>
struct UnicodeStringDescriptor {
  uint8_t bLength;
  DescriptorType bDescriptorType;
  const wchar_t bString[T];
};

// Setup packet format as defined by the USB spec rev. 2.0, section 9.3, table
// 9-3.
class SetupPacket {
 public:
  RequestType bmRequestType;
  Request bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;

  static SetupPacket ParseFromUsbEndpoint(native::Native *);

  bool operator==(const SetupPacket &other) const {
    return bmRequestType.GetValue() == other.bmRequestType.GetValue() &&
           bRequest == other.bRequest && wValue == other.wValue &&
           wIndex == other.wIndex && wLength == other.wLength;
  }
};
}  // namespace usb
}  // namespace threeboard
