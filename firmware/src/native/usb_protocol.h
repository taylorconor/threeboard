#pragma once

#include <stdint.h>

namespace threeboard {
namespace native {

// RequestType bitmap format as defined by the USB spec rev. 2.0, section 9.3,
// table 9-2.
class RequestType {
public:
  RequestType() {}
  RequestType(volatile uint8_t &value) : value_(value) {}
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

  Direction GetDirection() { return (Direction)(value_ >> 7); }
  Type GetType() { return (Type)((value_ >> 5) & 3); }
  Recipient GetRecipient() { return (Recipient)(value_ & 31); }

private:
  uint8_t value_;
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

// Descriptor value field of the descriptor, stored in wValue. Defined by the
// USB spec rev. 2.0, section 9.4.3.
class DescriptorValue {
public:
  // We need explicit default constructor here to allow Descriptor to remain an
  // aggregate, but it serves no other purpose.
  DescriptorValue() = default;

  // Descriptor type in the high byte, descriptor index in the low byte.
  constexpr DescriptorValue(const DescriptorType &type, uint8_t index)
      : value_(((uint8_t)type << 8) | index) {}

  // Also allow implicit initialization from a regular uint16_t.
  constexpr DescriptorValue(const uint16_t &value) : value_(value) {}

  bool operator==(const DescriptorValue &other) const {
    return this->value_ == other.value_;
  }

private:
  uint16_t value_;
};

// The GET_DESCRIPTOR request payload, as defined by the USB spec rev. 2.0,
// section 9.4.3.
struct DescriptorContainer {
  DescriptorValue wValue;
  uint16_t wIndex;
  const uint8_t *addr;
  uint8_t length;

  static DescriptorContainer ParseFromProgmem(const uint8_t *byte_ptr);
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

  static SetupPacket ParseFromUsbEndpoint();

private:
  SetupPacket() {}
};
} // namespace native
} // namespace threeboard
