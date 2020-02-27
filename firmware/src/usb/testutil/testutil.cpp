#include "testutil.h"

namespace threeboard {
namespace usb {
namespace testutil {
namespace {

using ::testing::Return;
}

void MockSendingSetupPacket(native::NativeMock *native_mock,
                            const SetupPacket &packet) {
  EXPECT_CALL(*native_mock, GetUEDATX())
      .Times(8)
      .WillOnce(Return(packet.bmRequestType.GetValue()))
      .WillOnce(Return((uint8_t)packet.bRequest))
      .WillOnce(Return((uint8_t)packet.wValue))
      .WillOnce(Return((uint8_t)(packet.wValue >> 8)))
      .WillOnce(Return((uint8_t)packet.wIndex))
      .WillOnce(Return((uint8_t)(packet.wIndex >> 8)))
      .WillOnce(Return((uint8_t)packet.wLength))
      .WillOnce(Return((uint8_t)(packet.wLength >> 8)));
}
} // namespace testutil
} // namespace usb
} // namespace threeboard
