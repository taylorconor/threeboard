#include "fake_host.h"

#include "gmock/gmock.h"

#include "src/usb/shared/protocol.h"

namespace threeboard {
namespace usb {
namespace testutil {
namespace {

using ::testing::InvokeWithoutArgs;
using ::testing::Return;

void SetupEnumerationExpectations(native::NativeMock *native_mock) {
  EXPECT_CALL(*native_mock, SetUHWCON(1 << native::UVREGE)).Times(1);
  EXPECT_CALL(*native_mock,
              SetUSBCON((1 << native::USBE) | (1 << native::FRZCLK)))
      .Times(1);
  EXPECT_CALL(*native_mock,
              SetPLLCSR((1 << native::PINDIV) | (1 << native::PLLE)))
      .Times(1);
  // Simulate PLL lock to reference clock.
  EXPECT_CALL(*native_mock, GetPLLCSR())
      .Times(1)
      .WillOnce(Return(1 << native::PLOCK));
  EXPECT_CALL(*native_mock,
              SetUSBCON((1 << native::USBE) | (1 << native ::OTGPADE)))
      .Times(1);
  EXPECT_CALL(*native_mock, GetUDCON()).Times(1).WillOnce(Return(99));
  EXPECT_CALL(*native_mock, SetUDCON(99 & ~(1 << native::DETACH))).Times(1);
  EXPECT_CALL(*native_mock,
              SetUDIEN((1 << native::EORSTE) | (1 << native::SOFE)))
      .Times(1);
}
} // namespace

FakeHost::FakeHost(native::NativeMock *native_mock, UsbImpl *usb_impl)
    : native_mock_(native_mock), usb_impl_(usb_impl) {}

void FakeHost::HandleDeviceEnumeration() {
  SetupEnumerationExpectations(native_mock_);
  EXPECT_CALL(*native_mock_, GetUEINTX())
      .Times(2)
      .WillOnce(Return(1 << native::RXSTPI))
      .WillOnce(Return(99));
  EXPECT_CALL(*native_mock_,
              SetUEINTX(99 & ~((1 << native::RXSTPI) | (1 << native::RXOUTI) |
                               (1 << native::TXINI))))
      .Times(1);
  AddFakeEndpointExpectations(Request::GET_DESCRIPTOR);
  usb_impl_->HandleEndpointInterrupt();
}

void FakeHost::AddFakeEndpointExpectations(Request request) {
  EXPECT_CALL(*native_mock_, SetUENUM(0)).Times(1);
  SetupPacket packet;
  packet.bmRequestType = 0;
  packet.bRequest = request;
  packet.wValue = DescriptorId(DescriptorType::DEVICE, 0).GetValue();
  packet.wLength = 255;
  EXPECT_CALL(*native_mock_, GetUEDATX())
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
