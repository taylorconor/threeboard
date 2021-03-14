#include "usb_host.h"

#include <unistd.h>

#include <iostream>

#include "src/usb/shared/constants.h"

namespace threeboard {
namespace simulator {

using namespace std::placeholders;
using usb::RequestType;

UsbHost::UsbHost(Simavr *simavr, SimulatorDelegate *simulator_delegate)
    : simavr_(simavr),
      simulator_delegate_(simulator_delegate),
      is_running_(false),
      is_attached_(false) {
  // Register a callback on USB attach, so we'll know when we try to start the
  // host if the device is ready or not.
  usb_attach_callback_ = std::make_unique<UsbAttachCallback>(
      std::bind(&UsbHost::InternalUsbAttachCallback, this, _1));
  usb_attach_lifetime_ =
      simavr_->RegisterUsbAttachCallback(usb_attach_callback_.get());
}

UsbHost::~UsbHost() {
  if (is_running_) {
    is_running_ = false;
    device_control_thread_->join();
  }
}

bool UsbHost::IsAttached() { return is_attached_; }

// This function is run exclusively within the device_control_thread_ thread.
void UsbHost::DeviceControlLoop() {
  is_running_ = true;

  // Before properly beginning the device control loop, we need to issue a USB
  // reset to ensure that the threeboard and simavr are configured correctly.
  simavr_->InvokeIoctl(USB_RESET, nullptr);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // Begin the device control loop and continue until the host is no longer
  // needed. This loop is responsible for configuring the keyboard with the
  // initial setup packets, and then polling it for keypress updates.
  // We can skip many of the unnecessary requests that a normal host would make,
  // since we know exactly what kind of device we're dealing with and its
  // capabilities.
  while (is_running_ && is_attached_) {
    // If the device has not configured yet (i.e. it has not yet set its
    // endpoint number, stored in UENUM, to the keyboard endpoint), configure
    // its keyboard endpoint here.
    if (simavr_->GetData(UENUM) != usb::descriptor::kKeyboardEndpoint) {
      usb::SetupPacket packet;
      packet.bmRequestType = RequestType(RequestType::Direction::HOST_TO_DEVICE,
                                         RequestType::Type::CLASS,
                                         RequestType::Recipient::ENDPOINT);
      packet.bRequest = usb::Request::SET_CONFIGURATION;
      packet.wValue = usb::descriptor::kConfigurationValue;
      // TODO: handle error
      UsbPacketBuffer packet_buffer = {.endpoint = 0,
                                       .size = sizeof(usb::SetupPacket),
                                       .buffer = (uint8_t *)&packet};
      simavr_->InvokeIoctl(USB_SETUP, &packet_buffer);
    } else {
      uint8_t read_buffer[8];
      UsbPacketBuffer packet_buffer = {
          .endpoint = usb::descriptor::kKeyboardEndpoint,
          .size = 8,
          .buffer = (uint8_t *)&read_buffer};
      int ret = simavr_->InvokeIoctl(USB_READ, &packet_buffer);
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
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
  }
}

void UsbHost::InternalUsbAttachCallback(uint32_t status) {
  // Verify that attaching was successful.
  if (status == 0) {
    if (is_attached_) {
      is_attached_ = false;
    }

    // TODO: perhaps we should surface an error here if no attach callback is
    // called quickly after with a valid status.
    return;
  }

  // Once we attach we can begin the device control thread and start the USB
  // protocol.
  if (!is_attached_) {
    is_attached_ = true;
    device_control_thread_ =
        std::make_unique<std::thread>(&UsbHost::DeviceControlLoop, this);
  }
}
}  // namespace simulator
}  // namespace threeboard
