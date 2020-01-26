#include "usb_protocol.h"

#include <avr/io.h>

namespace threeboard {
namespace native {

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
