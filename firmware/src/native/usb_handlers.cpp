#include "usb_handlers.h"

#include "../util/common.h"
#include "usb_descriptors.h"

#define ENABLE_ERR() ((PORTF |= 1 << PF6) && (PORTF &= ~(1 << PF7)))
#define ENABLE_STATUS() ((PORTF |= 1 << PF7) && (PORTF &= ~(1 << PF6)))

namespace threeboard {
namespace native {
namespace {

static constexpr uint8_t GetDescriptorListSize() {
  return sizeof(descriptor_list) / sizeof(DescriptorContainer);
}

static __always_inline void AwaitTransmitterReady() {
  while (!(UEINTX & (1 << TXINI)))
    ;
}
static __always_inline void HandshakeTransmitterInterrupt() {
  UEINTX = ~(1 << TXINI);
}
} // namespace

namespace device_handler {
namespace {

// Find the DescriptorContainer from the descriptor_list in PROGMEM that matches
// the DescriptorType of this packet.
bool FindMatchingContainer(const SetupPacket &packet,
                           DescriptorContainer *descriptor) {
  const DescriptorContainer *ptr = descriptor_list;
  // Find the first descriptor with a matching descriptor value.
  uint8_t i = 0;
  for (; i < GetDescriptorListSize(); i++, ptr++) {
    DescriptorId id = pgm_read_word(ptr);
    if (id == packet.wValue) {
      *descriptor = DescriptorContainer::ParseFromProgmem((uint8_t *)ptr);
      return true;
    }
  }
  /*
  // Now find the descriptor in the list with a matching wIndex.
  for (; i < GetDescriptorListSize(); i++, ptr++) {
    *descriptor = DescriptorContainer::ParseFromProgmem((uint8_t *)ptr);
    // Currently we have no use for the descriptor index, since we only support
    // one language, so it's safe to return if we see a descriptor index 0 (e.g.
    // the first string descriptor). This may become necessary to implement
    // properly in the future though.
    if (descriptor->index == 0 || descriptor->index == packet.wIndex) {
      return true;
    }
    }*/
  return false;
}
} // namespace

// Called when the host requests the status of the device.
void HandleGetStatus() {
  AwaitTransmitterReady();
  // The response only contains two status bits. Bit 0 = 0 indicates that this
  // device is not self-powered (it relies on bus power), and bit 1 = 0
  // indicates that the device does not support remote wakeup. We send this
  // response directly into the data register.
  UEDATX = 0;
  HandshakeTransmitterInterrupt();
}

// Allows the host to set the USB address of this device.
void HandleSetAddress(const SetupPacket &packet) {
  // TODO: USB spec section 9.4.6 specifies different behaviours here for
  // default and address state. Might need to implement this.
  HandshakeTransmitterInterrupt();
  // wValue contains the 7-bit address. The highest-order bit of the request
  // is unspecified.
  UDADDR = packet.wValue;
  // TODO: write a good comment why we need to do this.
  AwaitTransmitterReady();
  // Enable the address by setting the highest-order bit (a feature of the
  // microcontroller, not the USB protocol).
  UDADDR |= 1 << ADDEN;
}

// Returns a descriptor as requested by the host, if such a descriptor exists.
void HandleGetDescriptor(const SetupPacket &packet) {
  DescriptorContainer container;
  bool found = FindMatchingContainer(packet, &container);
  if (!found) {
    // Stall if we can't find a matching DescriptorContainer. This is an
    // unrecoverable error.
    UECONX = (1 << STALLRQ) | (1 << EPEN);
    return;
  }

  // Send the descriptor to the host.
  uint16_t remaining_packet_length =
      util::min(util::min(packet.wLength, 255), container.length);
  uint16_t current_frame_length = 0;
  while (remaining_packet_length > 0 ||
         current_frame_length == kEndpoint32ByteBank) {
    AwaitTransmitterReady();
    current_frame_length =
        util::min(remaining_packet_length, kEndpoint32ByteBank);
    for (uint16_t i = current_frame_length; i > 0; i--) {
      UEDATX = pgm_read_byte(container.data++);
    }
    remaining_packet_length -= current_frame_length;
    HandshakeTransmitterInterrupt();
  }
}

// Replies with the current Configuration of the device.
void HandleGetConfiguration(const UsbHidState &hid_state) {
  // TODO: Section 9.4.2 specifies different configs for different states.
  // Should also implement that here.
  AwaitTransmitterReady();
  UEDATX = hid_state.configuration;
  HandshakeTransmitterInterrupt();
}

// Allows the host to specify the current Configuration of the device.
void HandleSetConfiguration(const SetupPacket &packet, UsbHidState *hid_state) {
  HandshakeTransmitterInterrupt();
  hid_state->configuration = packet.wValue;

  // Make sure the host isn't trying to set us in a configuration we don't
  // support.
  if (hid_state->configuration != kKeyboardConfigurationValue) {
    return;
  }

  // Configure the only endpoint needed for the threeboard, the
  // interrupt-based keyboard endpoint.
  UENUM = kKeyboardEndpoint;
  UECONX = 1 << EPEN;
  UECFG0X = kEndpointTypeInterrupt | kEndpointDirectionIn;
  UECFG1X = kEndpointDoubleBank;

  // Reset the keyboard endpoint to enable it.
  UERST = 1 << kKeyboardEndpoint;
  UERST = 0;
}
} // namespace device_handler

namespace hid_handler {

// Replies with the state of the keyboard keys and modifier keys. Response
// protocol defined by HID spec v1.11, section B.1.
// TODO: this will always return zeroes, since the state is send to the host in
// UsbImpl. Perhaps this will cause a rare race condition where keypresses are
// missed?
void HandleGetReport(const UsbHidState &hid_state) {
  AwaitTransmitterReady();
  UEDATX = hid_state.modifier_keys;
  UEDATX = 0;
  for (uint8_t i = 0; i < 6; i++) {
    UEDATX = hid_state.keyboard_keys[i];
  }
  HandshakeTransmitterInterrupt();
}

// Get the idle config of the device.
void HandleGetIdle(const UsbHidState &hid_state) {
  AwaitTransmitterReady();
  UEDATX = hid_state.idle_config;
  HandshakeTransmitterInterrupt();
}

// Set the idle config of the device. We don't take any action from this, but we
// need to be able to get and set it.
void HandleSetIdle(const SetupPacket &packet, UsbHidState *hid_state) {
  HandshakeTransmitterInterrupt();
  hid_state->idle_config = (packet.wValue >> 8);
  hid_state->idle_count = 0;
}

// Get the current HID protocol. We only use one.
void HandleGetProtocol(const UsbHidState &hid_state) {
  AwaitTransmitterReady();
  UEDATX = hid_state.protocol;
  HandshakeTransmitterInterrupt();
}

// Set the current HID protocol.
void HandleSetProtocol(const SetupPacket &packet, UsbHidState *hid_state) {
  HandshakeTransmitterInterrupt();
  hid_state->protocol = packet.wValue;
}
} // namespace hid_handler
} // namespace native
} // namespace threeboard
