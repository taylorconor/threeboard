#include "usb_impl.h"

#include "src/delegates/usb_interrupt_handler_delegate.h"
#include "src/native/native_mock.h"
#include "src/usb/fake_host.h"
#include "gmock/gmock.h"

namespace threeboard {
namespace usb {
namespace {

using ::testing::_;
using ::testing::Return;

class UsbImplTest : public ::testing::Test {
public:
  UsbImplTest() {
    EXPECT_CALL(native_mock_, SetUsbInterruptHandlerDelegate(_)).Times(1);
    usb_ = std::make_unique<UsbImpl>(&native_mock_);
    fake_host_ =
        std::make_unique<testutil::FakeHost>(&native_mock_, usb_.get());
  }

  void VerifyEnumeration() {
    fake_host_->HandleDeviceEnumeration();
    usb_->Setup();
  }

  native::NativeMock native_mock_;
  std::unique_ptr<testutil::FakeHost> fake_host_;
  std::unique_ptr<UsbImpl> usb_;
};

TEST_F(UsbImplTest, EnumeratesCorrectly) {
  VerifyEnumeration();
  EXPECT_TRUE(usb_->HasConfigured());
}
} // namespace
} // namespace usb
} // namespace threeboard
