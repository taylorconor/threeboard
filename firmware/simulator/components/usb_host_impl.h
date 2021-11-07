#pragma once

#include <functional>
#include <thread>

#include "simulator/simavr/simavr.h"
#include "simulator/simulator_delegate.h"
#include "src/usb/shared/protocol.h"
#include "usb_host.h"

namespace threeboard {
namespace simulator {

// A class which facilitates interaction with the simulated threeboard over USB
// by acting as the USB host. It does the bare minimum to trick the threeboard
// into thinking it's connected to an actual USB host, and provides methods to
// interact with the simulated threeboard over USB. This host is in no way USB
// conformant.
class UsbHostImpl : public UsbHost {
 public:
  UsbHostImpl(Simavr *simavr, SimulatorDelegate *simulator_delegate);
  ~UsbHostImpl() override;

  bool IsAttached() const override;

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
}  // namespace simulator
}  // namespace threeboard
