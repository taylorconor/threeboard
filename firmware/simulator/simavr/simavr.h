#pragma once

#include "simulator/simavr/internal/iom32u4.h"
#include <functional>
#include <stdint.h>

namespace threeboard {
namespace simulator {

enum State {
  LIMBO,
  STOPPED,
  RUNNING,
  SLEEPING,
  STEP,
  STEP_DONE,
  DONE,
  CRASHED,
};

// Define all necessary IOCTL indices here so we don't have to depend on any
// simavr headers. The strange looking constants are an artifact of how simavr
// identifies IOCTL indices. They're defined in avr_usb.h.
enum Ioctl {
  USB_WRITE = 1970496119,
  USB_READ = 1970496114,
  USB_SETUP = 1970496115,
  USB_RESET = 1970496082,
  USB_VBUS = 1970496086,
};

// A copy of the avr_io_usb, defined in avr_usb.h.
struct UsbPacketBuffer {
  uint8_t endpoint;
  uint32_t size;
  uint8_t *buffer;
};

using UsbAttachCallback = std::function<void(uint32_t)>;

// A shim interface to collect the simavr API into one single interface to make
// all classes that interact with simavr testable.
class Simavr {
public:
  virtual ~Simavr() = default;

  virtual void Run() = 0;

  virtual void InitGdb() = 0;
  virtual void DeinitGdb() = 0;
  virtual void Reset() = 0;
  virtual void Terminate() = 0;

  virtual int InvokeIoctl(uint32_t ioctl, void *param) = 0;
  virtual void RegisterUsbAttachCallback(UsbAttachCallback *callback) = 0;

  virtual void SetData(uint8_t idx, uint8_t val) = 0;
  virtual void SetState(uint8_t val) = 0;
  virtual void SetGdbPort(uint8_t val) = 0;

  virtual uint8_t GetData(uint8_t idx) = 0;
  virtual uint8_t GetState() = 0;
  virtual uint8_t GetGdbPort() = 0;
  virtual uint64_t GetCycle() = 0;
};
} // namespace simulator
} // namespace threeboard

// elf_read_firmware
// avr_terminate
// avr_make_mcu_by_name
// avr_init
// avr_load_firmware
// -avr_deinit_gdb
// -avr_gdb_init
// avr_reset
// -avr_run
