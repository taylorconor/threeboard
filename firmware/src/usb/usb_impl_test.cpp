#include "usb_impl.h"

#include "src/delegates/usb_interrupt_handler_delegate.h"
#include "src/native/native_mock.h"

namespace threeboard {
namespace usb {
namespace {

using testing::_;
using testing::Return;

class UsbImplTest : public ::testing::Test {
public:
  UsbImplTest() { usb_ = std::make_unique<UsbImpl>(&native_mock_); }

  void Enumerate() {
    // Exhaustively set all expectations for setup/enumeration. For the most
    // part these are a copy of the implementation in usb_impl.cpp, but there's
    // still value in checking that it's working as expected.
    EXPECT_CALL(native_mock_, SetUHWCON(1 << native::UVREGE)).Times(1);
    EXPECT_CALL(native_mock_,
                SetUSBCON((1 << native::USBE) | (1 << native::FRZCLK)))
        .Times(1);
    EXPECT_CALL(native_mock_,
                SetPLLCSR((1 << native::PINDIV) | (1 << native::PLLE)))
        .Times(1);
    // Simulate PLL lock to reference clock.
    EXPECT_CALL(native_mock_, GetPLLCSR())
        .Times(1)
        .WillOnce(Return(1 << native::PLOCK));
    EXPECT_CALL(native_mock_,
                SetUSBCON((1 << native::USBE) | (1 << native ::OTGPADE)))
        .Times(1);
    EXPECT_CALL(native_mock_, GetUDCON()).Times(1).WillOnce(Return(99));
    EXPECT_CALL(native_mock_, SetUDCON(99 & ~(1 << native::DETACH)))
        .Times(1);
    EXPECT_CALL(native_mock_,
                SetUDIEN((1 << native::EORSTE) | (1 << native::SOFE)))
        .Times(1);

    usb_->Setup();
  }

  native::NativeMock native_mock_;
  std::unique_ptr<UsbImpl> usb_;
};

TEST_F(UsbImplTest, EnumeratesCorrectly) { Enumerate(); }
} // namespace
} // namespace usb
} // namespace threeboard
