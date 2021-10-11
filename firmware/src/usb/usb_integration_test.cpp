#include "gmock/gmock.h"
#include "src/delegates/error_handler_delegate_mock.h"
#include "src/delegates/usb_interrupt_handler_delegate.h"
#include "src/native/native_mock.h"
#include "src/usb/testutil/fake_host.h"
#include "usb_controller_impl.h"

namespace threeboard {
namespace usb {
namespace {

using ::testing::_;
using ::testing::Return;

class UsbImplTest : public ::testing::Test {
 public:
  UsbImplTest() {
    EXPECT_CALL(native_mock_, SetUsbInterruptHandlerDelegate(_)).Times(1);
    usb_controller_ = std::make_unique<UsbControllerImpl>(
        &native_mock_, &error_handler_delegate_mock_);
    fake_host_ = std::make_unique<testutil::FakeHost>(&native_mock_,
                                                      usb_controller_.get());
  }

  void VerifyEnumeration() {
    fake_host_->HandleDeviceEnumeration();
    usb_controller_->Setup();
  }

  native::NativeMock native_mock_;
  std::unique_ptr<testutil::FakeHost> fake_host_;
  std::unique_ptr<UsbControllerImpl> usb_controller_;
  ErrorHandlerDelegateMock error_handler_delegate_mock_;
};

TEST_F(UsbImplTest, EnumeratesCorrectly) {
  VerifyEnumeration();
  EXPECT_TRUE(usb_controller_->HasConfigured());
}
}  // namespace
}  // namespace usb
}  // namespace threeboard
