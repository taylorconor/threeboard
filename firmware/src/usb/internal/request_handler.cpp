#include "request_handler.h"

#include "src/usb/internal/descriptors.h"
#include "src/util/util.h"

namespace threeboard {
namespace usb {
namespace {

constexpr uint8_t GetDescriptorListSize() {
  return sizeof(descriptor_list) / sizeof(DescriptorContainer);
}

void AwaitTransmitterReady(native::Native *native) {
  while (!(native->GetUEINTX() & (1 << native::TXINI)))
    ;
}

void HandshakeTransmitterInterrupt(native::Native *native) {
  native->SetUEINTX(~(1 << native::TXINI));
}

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
  return false;
}
}  // namespace

RequestHandler::RequestHandler(native::Native *native) : native_(native) {}

// Called when the host requests the status of the device.
void RequestHandler::HandleGetStatus() {
  AwaitTransmitterReady(native_);
  // The response only contains two status bits. Bit 0 = 0 indicates that this
  // device is not self-powered (it relies on bus power), and bit 1 = 0
  // indicates that the device does not support remote wakeup. We send this
  // response directly into the data register.
  native_->SetUEDATX(0);
  HandshakeTransmitterInterrupt(native_);
}

// Allows the host to set the USB address of this device.
void RequestHandler::HandleSetAddress(const SetupPacket &packet) {
  // TODO: USB spec section 9.4.6 specifies different behaviours here for
  // default and address state. Might need to implement this.
  HandshakeTransmitterInterrupt(native_);
  // wValue contains the 7-bit address. The highest-order bit of the request
  // is unspecified.
  native_->SetUDADDR(packet.wValue);
  // TODO: write a good comment why we need to do this.
  AwaitTransmitterReady(native_);
  // Enable the address by setting the highest-order bit (a feature of the
  // microcontroller, not the USB protocol).
  native_->SetUDADDR(native_->GetUDADDR() | (1 << native::ADDEN));
}

// Returns a descriptor as requested by the host, if such a descriptor exists.
void RequestHandler::HandleGetDescriptor(const SetupPacket &packet) {
  DescriptorContainer container;
  bool found = FindMatchingContainer(native_, packet, &container);
  if (!found) {
    // Stall if we can't find a matching DescriptorContainer. This is an
    // unrecoverable error.
    native_->SetUECONX((1 << native::STALLRQ) | (1 << native::EPEN));
    return;
  }

  // Send the descriptor to the host.
  uint16_t remaining_packet_length =
      util::min(util::min(packet.wLength, 255), container.length);
  uint16_t current_frame_length = 0;
  while (remaining_packet_length > 0 ||
         current_frame_length == k32BytePacketSize) {
    AwaitTransmitterReady(native_);
    current_frame_length =
        util::min(remaining_packet_length, k32BytePacketSize);
    for (uint16_t i = current_frame_length; i > 0; i--) {
      native_->SetUEDATX(native_->ReadPgmByte(container.data++));
    }
    remaining_packet_length -= current_frame_length;
    HandshakeTransmitterInterrupt(native_);
  }
}

// Replies with the current Configuration of the device.
void RequestHandler::HandleGetConfiguration(const HidState &hid_state) {
  AwaitTransmitterReady(native_);
  native_->SetUEDATX(hid_state.configuration);
  HandshakeTransmitterInterrupt(native_);
}

// Allows the host to specify the current Configuration of the device.
void RequestHandler::HandleSetConfiguration(const SetupPacket &packet,
                                            HidState *hid_state) {
  HandshakeTransmitterInterrupt(native_);
  hid_state->configuration = packet.wValue;

  // Make sure the host isn't trying to set us in a configuration we don't
  // support.
  if (hid_state->configuration != descriptor::kConfigurationValue) {
    return;
  }

  // Configure the only endpoint needed for the threeboard, the
  // interrupt-based keyboard endpoint.
  native_->SetUENUM(descriptor::kKeyboardEndpoint);
  native_->SetUECONX(1 << native::EPEN);
  native_->SetUECFG0X(kEndpointTypeInterrupt | kEndpointDirectionIn);
  native_->SetUECFG1X(kEndpointDoubleBank | kEndpointAlloc);

  // Reset the keyboard endpoint to enable it.
  native_->SetUERST(1 << descriptor::kKeyboardEndpoint);
  native_->SetUERST(0);
}

// Replies with the state of the keyboard keys and modifier keys. Response
// protocol defined by HID spec v1.11, section B.1.
// TODO: this will always return zeroes, since the state is send to the host in
// UsbImpl. Perhaps this will cause a rare race condition where keypresses are
// missed?
void RequestHandler::HandleGetReport(const HidState &hid_state) {
  AwaitTransmitterReady(native_);
  native_->SetUEDATX(hid_state.modifier_keys);
  native_->SetUEDATX(0);
  for (auto &keyboard_key : hid_state.keyboard_keys) {
    native_->SetUEDATX(keyboard_key);
  }
  HandshakeTransmitterInterrupt(native_);
}

// Get the idle config of the device.
void RequestHandler::HandleGetIdle(const HidState &hid_state) {
  AwaitTransmitterReady(native_);
  native_->SetUEDATX(hid_state.idle_config);
  HandshakeTransmitterInterrupt(native_);
}

// Set the idle config of the device. It determines how frequently to send
// HidState on SOFI.
void RequestHandler::HandleSetIdle(const SetupPacket &packet,
                                   HidState *hid_state) {
  HandshakeTransmitterInterrupt(native_);
  hid_state->idle_config = (packet.wValue >> 8);
  hid_state->idle_count = 0;
}

// Get the current HID protocol. We only use one.
void RequestHandler::HandleGetProtocol(const HidState &hid_state) {
  AwaitTransmitterReady(native_);
  native_->SetUEDATX(hid_state.protocol);
  HandshakeTransmitterInterrupt(native_);
}

// Set the current HID protocol.
void RequestHandler::HandleSetProtocol(const SetupPacket &packet,
                                       HidState *hid_state) {
  HandshakeTransmitterInterrupt(native_);
  hid_state->protocol = packet.wValue;
}
}  // namespace usb
}  // namespace threeboard
