#include "protocol.h"

namespace threeboard {
namespace usb {

SetupPacket SetupPacket::ParseFromUsbEndpoint(native::Native *native) {
  // Switch to endpoint 0.
  native->SetUENUM(0);
  // Construct a SetupPacket from data on the bank.
  SetupPacket packet;
  packet.bmRequestType = native->GetUEDATX();
  packet.bRequest = (Request)native->GetUEDATX();
  packet.wValue = native->GetUEDATX();
  packet.wValue |= (native->GetUEDATX() << 8);
  packet.wIndex = native->GetUEDATX();
  packet.wIndex |= (native->GetUEDATX() << 8);
  packet.wLength = native->GetUEDATX();
  packet.wLength |= (native->GetUEDATX() << 8);
  return packet;
}

}  // namespace usb
}  // namespace threeboard
