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
using UsbAttachCallback = std::function<void(uint32_t)>;
using usb::RequestType;

// C-style trampoline function to bounce the avr_irq_register_notify callback to
// the provided UsbAttachCallback callback.
void UsbAttachCallbackTrampoline(avr_irq_t *irq, uint32_t value, void *param) {
  auto *callback = (UsbAttachCallback *)param;
  (*callback)(value);
}
} // namespace

Host::Host(avr_t *avr, SimulatorDelegate *simulator_delegate)
    : avr_(avr), simulator_delegate_(simulator_delegate), is_running_(false) {
  // avr_->log = 4;

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
  // TODO: get AVR_IOCTL_USB_RESET_AND_SOFI into master!
  avr_ioctl(avr_, AVR_IOCTL_USB_RESET, nullptr);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // Begin the device control loop and continue until the host is no longer
  // needed. This loop is responsible for configuring the keyboard with the
  // initial setup packets, and then polling it for keypress updates.
  // We can skip many of the unnecessary requests that a normal host would make,
  // since we know exactly what kind of device we're dealing with and its
  // capabilities.
  while (is_running_) {
    // If the device has not configured yet (i.e. it has not yet set its
    // endpoint number, stored in UENUM, to the keyboard endpoint), configure
    // its keyboard endpoint here.
    if (avr_->data[UENUM] != usb::kKeyboardEndpoint) {
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
    } else {
      uint8_t read_buffer[8];
      avr_io_usb packet_buffer = {.pipe = usb::kKeyboardEndpoint,
                                  .sz = 8,
                                  .buf = (uint8_t *)&read_buffer};
      int ret = avr_ioctl(avr_, AVR_IOCTL_USB_READ, &packet_buffer);
      if (ret == 0) {
        // Keypress changes are sent for two reasons: first when the key is
        // pressed down, and then when the key is released. Right now it doesn't
        // matter when we register the keypress in the simulator so we do it on
        // key down.
        if (read_buffer[0] != 0 || read_buffer[2] != 0) {
          simulator_delegate_->HandleVirtualKeypress(read_buffer[0],
                                                     read_buffer[2]);
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
