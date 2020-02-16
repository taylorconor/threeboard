#include "handlers.h"

#include "src/usb/descriptors.h"
#include "src/util/util.h"

namespace threeboard {
namespace usb {
namespace {

static constexpr uint8_t GetDescriptorListSize() {
  return sizeof(descriptor_list) / sizeof(DescriptorContainer);
}

static __always_inline void AwaitTransmitterReady(native::Native *native) {
  while (!(native->GetUEINTX() & (1 << native::TXINI)))
    ;
}
static __always_inline void
HandshakeTransmitterInterrupt(native::Native *native) {
  native->SetUEINTX(~(1 << native::TXINI));
}
} // namespace

namespace device_handler {
namespace {

// Find the DescriptorContainer from the descriptor_list in PROGMEM that matches
// the DescriptorType of this packet.
bool FindMatchingContainer(native::Native *native, const SetupPacket &packet,
                           DescriptorContainer *descriptor) {
  const DescriptorContainer *ptr = descriptor_list;
  // Find the first descriptor with a matching descriptor value.
  uint8_t i = 0;
  for (; i < GetDescriptorListSize(); i++, ptr++) {
    DescriptorId id = native->ReadPgmWord((uint8_t *)ptr);
    if (id == packet.wValue) {
      *descriptor =
          DescriptorContainer::ParseFromProgmem(native, (uint8_t *)ptr);
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
void HandleGetStatus(native::Native *native) {
  AwaitTransmitterReady(native);
  // The response only contains two status bits. Bit 0 = 0 indicates that this
  // device is not self-powered (it relies on bus power), and bit 1 = 0
  // indicates that the device does not support remote wakeup. We send this
  // response directly into the data register.
  native->SetUEDATX(0);
  HandshakeTransmitterInterrupt(native);
}

// Allows the host to set the USB address of this device.
void HandleSetAddress(native::Native *native, const SetupPacket &packet) {
  // TODO: USB spec section 9.4.6 specifies different behaviours here for
  // default and address state. Might need to implement this.
  HandshakeTransmitterInterrupt(native);
  // wValue contains the 7-bit address. The highest-order bit of the request
  // is unspecified.
  native->SetUDADDR(packet.wValue);
  // TODO: write a good comment why we need to do this.
  AwaitTransmitterReady(native);
  // Enable the address by setting the highest-order bit (a feature of the
  // microcontroller, not the USB protocol).
  native->SetUDADDR(native->GetUDADDR() | (1 << native::ADDEN));
}

// Returns a descriptor as requested by the host, if such a descriptor exists.
void HandleGetDescriptor(native::Native *native, const SetupPacket &packet) {
  DescriptorContainer container;
  bool found = FindMatchingContainer(native, packet, &container);
  if (!found) {
    // Stall if we can't find a matching DescriptorContainer. This is an
    // unrecoverable error.
    native->SetUECONX((1 << native::STALLRQ) | (1 << native::EPEN));
    return;
  }

  // Send the descriptor to the host.
  uint16_t remaining_packet_length =
      util::min(util::min(packet.wLength, 255), container.length);
  uint16_t current_frame_length = 0;
  while (remaining_packet_length > 0 ||
         current_frame_length == kEndpoint32ByteBank) {
    AwaitTransmitterReady(native);
    current_frame_length =
        util::min(remaining_packet_length, kEndpoint32ByteBank);
    for (uint16_t i = current_frame_length; i > 0; i--) {
      native->SetUEDATX(native->ReadPgmByte(container.data++));
    }
    remaining_packet_length -= current_frame_length;
    HandshakeTransmitterInterrupt(native);
  }
}

// Replies with the current Configuration of the device.
void HandleGetConfiguration(native::Native *native, const HidState &hid_state) {
  // TODO: Section 9.4.2 specifies different configs for different states.
  // Should also implement that here.
  AwaitTransmitterReady(native);
  native->SetUEDATX(hid_state.configuration);
  HandshakeTransmitterInterrupt(native);
}

// Allows the host to specify the current Configuration of the device.
void HandleSetConfiguration(native::Native *native, const SetupPacket &packet,
                            HidState *hid_state) {
  HandshakeTransmitterInterrupt(native);
  hid_state->configuration = packet.wValue;

  // Make sure the host isn't trying to set us in a configuration we don't
  // support.
  if (hid_state->configuration != kKeyboardConfigurationValue) {
    return;
  }

  // Configure the only endpoint needed for the threeboard, the
  // interrupt-based keyboard endpoint.
  native->SetUENUM(kKeyboardEndpoint);
  native->SetUECONX(1 << native::EPEN);
  native->SetUECFG0X(kEndpointTypeInterrupt | kEndpointDirectionIn);
  native->SetUECFG1X(kEndpointDoubleBank);

  // Reset the keyboard endpoint to enable it.
  native->SetUERST(1 << kKeyboardEndpoint);
  native->SetUERST(0);
}
} // namespace device_handler

namespace hid_handler {

// Replies with the state of the keyboard keys and modifier keys. Response
// protocol defined by HID spec v1.11, section B.1.
// TODO: this will always return zeroes, since the state is send to the host in
// UsbImpl. Perhaps this will cause a rare race condition where keypresses are
// missed?
void HandleGetReport(native::Native *native, const HidState &hid_state) {
  AwaitTransmitterReady(native);
  native->SetUEDATX(hid_state.modifier_keys);
  native->SetUEDATX(0);
  for (uint8_t i = 0; i < 6; i++) {
    native->SetUEDATX(hid_state.keyboard_keys[i]);
  }
  HandshakeTransmitterInterrupt(native);
}

// Get the idle config of the device.
void HandleGetIdle(native::Native *native, const HidState &hid_state) {
  AwaitTransmitterReady(native);
  native->SetUEDATX(hid_state.idle_config);
  HandshakeTransmitterInterrupt(native);
}

// Set the idle config of the device. We don't take any action from this, but we
// need to be able to get and set it.
void HandleSetIdle(native::Native *native, const SetupPacket &packet,
                   HidState *hid_state) {
  HandshakeTransmitterInterrupt(native);
  hid_state->idle_config = (packet.wValue >> 8);
  hid_state->idle_count = 0;
}

// Get the current HID protocol. We only use one.
void HandleGetProtocol(native::Native *native, const HidState &hid_state) {
  AwaitTransmitterReady(native);
  native->SetUEDATX(hid_state.protocol);
  HandshakeTransmitterInterrupt(native);
}

// Set the current HID protocol.
void HandleSetProtocol(native::Native *native, const SetupPacket &packet,
                       HidState *hid_state) {
  HandshakeTransmitterInterrupt(native);
  hid_state->protocol = packet.wValue;
}
} // namespace hid_handler
} // namespace usb
} // namespace threeboard
