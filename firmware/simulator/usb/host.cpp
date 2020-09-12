#include "host.h"

#include <iostream>
#include <unistd.h>

#include "simavr/avr_usb.h"
#include "simulator/core/sim_32u4.h"
#include "src/usb/shared/constants.h"

namespace threeboard {
namespace simulator {
namespace {

using namespace std::placeholders;
using usb::RequestType;

// C-style trampoline function to bounce the avr_irq_register_notify callback to
// the provided UsbAttachCallback callback.
void UsbAttachCallbackTrampoline(avr_irq_t *irq, uint32_t value, void *param) {
  Host::UsbAttachCallback *callback = (Host::UsbAttachCallback *)param;
  (*callback)(value);
}
} // namespace

Host::Host(avr_t *avr) : avr_(avr) {
  avr_->log = 4;

  // Configure a callback on USB attach, so we'll know when we try to start the
  // host if the device is ready or not.
  usb_attach_callback_ = std::make_unique<UsbAttachCallback>(
      std::bind(&Host::InternalUsbAttachCallback, this, _1));
  avr_irq_register_notify(
      avr_io_getirq(avr_, AVR_IOCTL_USB_GETIRQ(), USB_IRQ_ATTACH),
      &UsbAttachCallbackTrampoline, (void *)usb_attach_callback_.get());
}

bool Host::IsRunning() { return is_running_; }

// This function is run exclusively within the device_control_thread_ thread.
void Host::DeviceControlLoop() {
  // Before properly beginning the device control loop, we need to issue a USB
  // reset to ensure that the threeboard and simavr are configured correctly.
  avr_ioctl(avr_, AVR_IOCTL_USB_RESET, NULL);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // Begin the device control loop and continue until the host is no longer
  // needed. This loop is responsible for configuring the keyboard with the
  // initial setup packets, and then polling it for keypress updates every 1ms.
  // We can skip many of the unnecessary requests that a normal host would make,
  // since we know exactly what kind of device we're dealing with and its
  // capabilities.
  while (is_running_) {
    auto processing_start = std::chrono::system_clock::now();

    // If the device has not configured yet (i.e. it has not yet set its
    // endpoint number UENUM to the keyboard endpoint), configure its keyboard
    // endpoint here.
    // TODO: re-enable when ready.
    if (0 && avr_->data[UENUM] != usb::kKeyboardEndpoint) {
      usb::SetupPacket packet;
      packet.bmRequestType = RequestType(RequestType::Direction::HOST_TO_DEVICE,
                                         RequestType::Type::CLASS,
                                         RequestType::Recipient::ENDPOINT);
      packet.bRequest = usb::Request::SET_CONFIGURATION;
      packet.wValue = usb::kKeyboardConfigurationValue;
      // TODO: handle error
      avr_io_usb packet_buffer = {
          .pipe = 0, .sz = sizeof(usb::SetupPacket), .buf = (uint8_t *)&packet};
      avr_ioctl(avr_, AVR_IOCTL_USB_SETUP, &packet_buffer);
    }

    auto processing_end = std::chrono::system_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(1) -
                                (processing_end - processing_start));
  }
}

void Host::InternalUsbAttachCallback(uint32_t status) {
  // Verify that attaching was successful.
  if (status == 0) {
    // TODO: perhaps we should surface an error here if no attach callback is
    // called quickly after with a valid status.
    return;
  }

  // Once we attach we can begin the device control thread and start the USB
  // protocol.
  if (!is_running_) {
    is_running_ = true;
    device_control_thread_ =
        std::make_unique<std::thread>(&Host::DeviceControlLoop, this);
  }
}
} // namespace simulator
} // namespace threeboard
