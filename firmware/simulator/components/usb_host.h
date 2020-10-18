#pragma once

#include <atomic>
#include <functional>
#include <thread>

#include "simulator/simavr/simavr.h"
#include "simulator/simulator_delegate.h"
#include "src/usb/shared/protocol.h"

namespace threeboard {
namespace simulator {

// A class which facilitates interaction with the simulated threeboard over USB
// by acting as the USB host. It does the bare minimum to trick the threeboard
// into thinking it's connected to an actual USB host, and provides methods to
// interact with the simulated threeboard over USB.
class UsbHost {
public:
  UsbHost(Simavr *simavr, SimulatorDelegate *simulator_delegate);
  ~UsbHost();

  bool IsAttached();

private:
  void DeviceControlLoop();
  void InternalUsbAttachCallback(uint32_t status);

  Simavr *simavr_;
  SimulatorDelegate *simulator_delegate_;

  std::atomic<bool> is_running_;
  std::atomic<bool> is_attached_;
  std::unique_ptr<std::thread> device_control_thread_;
  std::unique_ptr<UsbAttachCallback> usb_attach_callback_;
  std::unique_ptr<Lifetime> usb_attach_lifetime_;
};
} // namespace simulator
} // namespace threeboard
