#pragma once

#include <atomic>
#include <functional>
#include <thread>

#include "simavr/sim_avr.h"
#include "src/usb/shared/protocol.h"

namespace threeboard {
namespace simulator {

// A class which facilitates interaction with the simulated threeboard over USB
// by acting as the USB host. It does the bare minimum to trick the threeboard
// into thinking it's connected to an actual USB host, and provides methods to
// interact with the simulated threeboard over USB.
class Host {
public:
  using UsbAttachCallback = std::function<void(uint32_t)>;

  Host(avr_t *avr);

  bool Start();
  bool IsRunning();
  void HandleRegisterWrite(avr_io_addr_t addr, uint8_t val);

private:
  void DeviceControlLoop();

  void InternalUsbAttachCallback(uint32_t status);

  avr_t *avr_;
  std::atomic<bool> is_running_;
  std::unique_ptr<std::thread> device_control_thread_;
  std::unique_ptr<UsbAttachCallback> usb_attach_callback_;
};
} // namespace simulator
} // namespace threeboard
