#include "protocol.h"

#include <iostream>

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
  std::cout << "&&& wvalue 1 = " << packet.wValue << std::endl;
  packet.wValue |= (native->GetUEDATX() << 8);
  std::cout << "&&& wvalue 2 = " << packet.wValue << std::endl;
  packet.wIndex = native->GetUEDATX();
  packet.wIndex |= (native->GetUEDATX() << 8);
  packet.wLength = native->GetUEDATX();
  std::cout << "&&& wlength 1 = " << packet.wLength << std::endl;
  packet.wLength |= (native->GetUEDATX() << 8);
  std::cout << "&&& wlength 2 = " << packet.wLength << std::endl;
  return packet;
}

} // namespace usb
} // namespace threeboard
