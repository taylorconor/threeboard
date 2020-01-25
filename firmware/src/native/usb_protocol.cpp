#include "usb_protocol.h"

#include <avr/pgmspace.h>

namespace threeboard {
namespace native {

DescriptorContainer DescriptorContainer::ParseFromProgmem(const uint8_t *ptr) {
  DescriptorContainer descriptor;
  descriptor.id = pgm_read_word(ptr);
  descriptor.index = pgm_read_word(ptr + 2);
  descriptor.data = (const uint8_t *)pgm_read_word(ptr + 4);
  descriptor.length = pgm_read_byte(ptr + 6);
  return descriptor;
}

SetupPacket SetupPacket::ParseFromUsbEndpoint() {
  // Switch to endpoint 0.
  UENUM = 0;
  // Construct a SetupPacket from data on the bank.
  SetupPacket packet;
  packet.bmRequestType = UEDATX;
  packet.bRequest = (Request)UEDATX;
  packet.wValue = UEDATX;
  packet.wValue |= (UEDATX << 8);
  packet.wIndex = UEDATX;
  packet.wIndex |= (UEDATX << 8);
  packet.wLength = UEDATX;
  packet.wLength |= (UEDATX << 8);
  return packet;
}

} // namespace native
} // namespace threeboard
