#include "usb_controller_impl.h"

#include "src/logging_fake.h"
#include "src/native/native_mock.h"
#include "src/usb/internal/request_handler_mock.h"
#include "src/usb/testutil/testutil.h"

namespace threeboard {
namespace usb {

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

class UsbImplTest : public ::testing::Test {
 public:
  UsbImplTest() : handler_mock_(&native_mock_) {
    EXPECT_CALL(native_mock_, SetUsbInterruptHandlerDelegate(_)).Times(1);
    usb_controller_ = std::make_unique<UsbControllerImpl>(&native_mock_);
    usb_controller_->request_handler_ = &handler_mock_;
  }

  void SetupEndpointInterruptMocks() {
    EXPECT_CALL(native_mock_, GetUEINTX())
        .Times(2)
        .WillOnce(Return(1 << native::RXSTPI))
        .WillOnce(Return(99));
    EXPECT_CALL(native_mock_,
                SetUEINTX(99 & ~((1 << native::RXSTPI) | (1 << native::RXOUTI) |
                                 (1 << native::TXINI))))
        .Times(1);
    EXPECT_CALL(native_mock_, SetUENUM(0)).Times(1);
  }

  SetupPacket MockEndpointInterrupt(Request request, RequestType request_type,
                                    uint8_t index) {
    SetupEndpointInterruptMocks();
    SetupPacket packet = {
        .bmRequestType = request_type,
        .bRequest = request,
        .wValue = 0,
        .wIndex = index,
        .wLength = 18,
    };
    testutil::MockSendingSetupPacket(&native_mock_, packet);
    return packet;
  }

  SetupPacket MockEndpointInterrupt(Request request, RequestType request_type) {
    return MockEndpointInterrupt(request, request_type, 0);
  }

  SetupPacket MockEndpointInterrupt(Request request) {
    return MockEndpointInterrupt(request, 0, 0);
  }

  native::NativeMock native_mock_;
  RequestHandlerMock handler_mock_;
  LoggingFake logging_fake_;
  std::unique_ptr<UsbControllerImpl> usb_controller_;
};

TEST_F(UsbImplTest, SetupFailsOnPllLockFailure) {
  EXPECT_CALL(native_mock_, SetUHWCON(1 << native::UVREGE)).Times(1);
  EXPECT_CALL(native_mock_,
              SetUSBCON((1 << native::USBE) | (1 << native::FRZCLK)))
      .Times(1);
  EXPECT_CALL(native_mock_,
              SetPLLCSR((1 << native::PINDIV) | (1 << native::PLLE)))
      .Times(1);
  EXPECT_CALL(native_mock_, GetPLLCSR())
      .Times(UINT16_MAX)
      .WillRepeatedly(Return(0));
  EXPECT_EQ(usb_controller_->Setup(), false);
}

TEST_F(UsbImplTest, SetupSuccess) {
  EXPECT_CALL(native_mock_, SetUHWCON(1 << native::UVREGE)).Times(1);
  EXPECT_CALL(native_mock_,
              SetUSBCON((1 << native::USBE) | (1 << native::FRZCLK)))
      .Times(1);
  EXPECT_CALL(native_mock_,
              SetPLLCSR((1 << native::PINDIV) | (1 << native::PLLE)))
      .Times(1);
  EXPECT_CALL(native_mock_, GetPLLCSR()).WillOnce(Return(1 << native::PLOCK));
  EXPECT_CALL(native_mock_,
              SetUSBCON((1 << native::USBE) | (1 << native::OTGPADE)))
      .Times(1);
  EXPECT_CALL(native_mock_,
              SetUDIEN((1 << native::EORSTE) | (1 << native::SOFE)))
      .Times(1);
  EXPECT_CALL(native_mock_, GetUDCON()).WillOnce(Return(42));
  EXPECT_CALL(native_mock_, SetUDCON(42 & ~(1 << native::DETACH))).Times(1);
  EXPECT_EQ(usb_controller_->Setup(), true);
}

TEST_F(UsbImplTest, DoesNothingOnInvalidSetupPacket) {
  SetupEndpointInterruptMocks();
  EXPECT_CALL(native_mock_, GetUEDATX())
      .Times(8)
      .WillOnce(Return(0))    // bmRequestType
      .WillOnce(Return(255))  // bRequest
      .WillRepeatedly(Return(0));
  usb_controller_->HandleEndpointInterrupt();
}

TEST_F(UsbImplTest, StallsWithoutInterrupt) {
  EXPECT_CALL(native_mock_, GetUEINTX())
      .Times(2)
      .WillOnce(Return(0))
      .WillOnce(Return(99));
  EXPECT_CALL(native_mock_,
              SetUEINTX(99 & ~((1 << native::RXSTPI) | (1 << native::RXOUTI) |
                               (1 << native::TXINI))))
      .Times(1);
  EXPECT_CALL(native_mock_, SetUENUM(0)).Times(1);
  EXPECT_CALL(native_mock_,
              SetUECONX((1 << native::STALLRQ) | (1 << native::EPEN)))
      .Times(1);
  // TODO: We need to do this for now because we immediately parse the SETUP
  // packet on interrupt. Do we still need to do this?
  SetupPacket packet;
  testutil::MockSendingSetupPacket(&native_mock_, packet);
  usb_controller_->HandleEndpointInterrupt();
}

// Device requests.
TEST_F(UsbImplTest, HandlesGetStatusRequest) {
  MockEndpointInterrupt(Request::GET_STATUS);
  EXPECT_CALL(handler_mock_, HandleGetStatus()).Times(1);
  usb_controller_->HandleEndpointInterrupt();
}

TEST_F(UsbImplTest, HandleSetAddressRequest) {
  auto packet = MockEndpointInterrupt(Request::SET_ADDRESS);
  EXPECT_CALL(handler_mock_, HandleSetAddress(packet)).Times(1);
  usb_controller_->HandleEndpointInterrupt();
}

TEST_F(UsbImplTest, HandleGetDescriptorRequest) {
  auto packet = MockEndpointInterrupt(Request::GET_DESCRIPTOR);
  EXPECT_CALL(handler_mock_, HandleGetDescriptor(packet)).Times(1);
  usb_controller_->HandleEndpointInterrupt();
}

TEST_F(UsbImplTest, HandleGetConfigurationRequest) {
  MockEndpointInterrupt(Request::GET_CONFIGURATION,
                        {RequestType::Direction::DEVICE_TO_HOST});
  EXPECT_CALL(handler_mock_, HandleGetConfiguration(_)).Times(1);
  usb_controller_->HandleEndpointInterrupt();
}

TEST_F(UsbImplTest, HandleSetConfigurationRequest) {
  auto packet = MockEndpointInterrupt(Request::SET_CONFIGURATION,
                                      {RequestType::Direction::HOST_TO_DEVICE});
  EXPECT_CALL(handler_mock_, HandleSetConfiguration(packet, _)).Times(1);
  usb_controller_->HandleEndpointInterrupt();
}

// HID requests.
TEST_F(UsbImplTest, HandleGetReportRequest) {
  MockEndpointInterrupt(
      Request::HID_GET_REPORT,
      {RequestType::Direction::DEVICE_TO_HOST, RequestType::Type::CLASS,
       RequestType::Recipient::INTERFACE},
      descriptor::kKeyboardInterfaceIndex);
  EXPECT_CALL(handler_mock_, HandleGetReport(_)).Times(1);
  usb_controller_->HandleEndpointInterrupt();
}

TEST_F(UsbImplTest, HandleGetIdleRequest) {
  MockEndpointInterrupt(
      Request::HID_GET_IDLE,
      {RequestType::Direction::DEVICE_TO_HOST, RequestType::Type::CLASS,
       RequestType::Recipient::INTERFACE},
      descriptor::kKeyboardInterfaceIndex);
  EXPECT_CALL(handler_mock_, HandleGetIdle(_)).Times(1);
  usb_controller_->HandleEndpointInterrupt();
}

TEST_F(UsbImplTest, HandleGetProtocolRequest) {
  MockEndpointInterrupt(
      Request::HID_GET_PROTOCOL,
      {RequestType::Direction::DEVICE_TO_HOST, RequestType::Type::CLASS,
       RequestType::Recipient::INTERFACE},
      descriptor::kKeyboardInterfaceIndex);
  EXPECT_CALL(handler_mock_, HandleGetProtocol(_)).Times(1);
  usb_controller_->HandleEndpointInterrupt();
}

TEST_F(UsbImplTest, HandleSetIdleRequest) {
  auto packet = MockEndpointInterrupt(
      Request::HID_SET_IDLE,
      {RequestType::Direction::HOST_TO_DEVICE, RequestType::Type::CLASS,
       RequestType::Recipient::INTERFACE},
      descriptor::kKeyboardInterfaceIndex);
  EXPECT_CALL(handler_mock_, HandleSetIdle(packet, _)).Times(1);
  usb_controller_->HandleEndpointInterrupt();
}

TEST_F(UsbImplTest, HandleSetProtocolRequest) {
  auto packet = MockEndpointInterrupt(
      Request::HID_SET_PROTOCOL,
      {RequestType::Direction::HOST_TO_DEVICE, RequestType::Type::CLASS,
       RequestType::Recipient::INTERFACE},
      descriptor::kKeyboardInterfaceIndex);
  EXPECT_CALL(handler_mock_, HandleSetProtocol(packet, _)).Times(1);
  usb_controller_->HandleEndpointInterrupt();
}
}  // namespace usb
}  // namespace threeboard
