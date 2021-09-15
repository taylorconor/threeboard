#pragma once

#include <stdint.h>

#include <functional>
#include <memory>

#include "simulator/simavr/internal/iom32u4.h"
#include "simulator/simavr/internal/simavr_headers.h"
#include "simulator/util/lifetime.h"

namespace threeboard {
namespace simulator {

using UsbAttachCallback = std::function<void(uint32_t)>;
using UartOutputCallback = std::function<void(uint8_t)>;
using I2cMessageCallback = std::function<void(uint32_t)>;
using PortWriteCallback = std::function<void(uint8_t)>;

// A shim to collect the simavr API into one single interface to make all
// classes that interact with simavr testable.
class Simavr {
 public:
  virtual ~Simavr() = default;

  virtual void Run() = 0;

  virtual void InitGdb() = 0;
  virtual void DeinitGdb() = 0;
  virtual void Reset() = 0;
  virtual void Terminate() = 0;

  virtual int InvokeIoctl(uint32_t ioctl, void *param) = 0;

  virtual std::unique_ptr<Lifetime> RegisterUsbAttachCallback(
      UsbAttachCallback *callback) = 0;
  virtual std::unique_ptr<Lifetime> RegisterUartOutputCallback(
      UartOutputCallback *callback) = 0;
  virtual std::unique_ptr<Lifetime> RegisterI2cMessageCallback(
      I2cMessageCallback *callback) = 0;
  virtual void RegisterPortBWriteCallback(PortWriteCallback *callback) = 0;
  virtual void RegisterPortDWriteCallback(PortWriteCallback *callback) = 0;

  virtual void RaiseI2cIrq(uint8_t direction, uint32_t value) = 0;

  virtual void SetData(uint8_t idx, uint8_t val) = 0;
  virtual void SetState(uint8_t val) = 0;
  virtual void SetGdbPort(uint16_t val) = 0;

  virtual uint8_t GetData(uint8_t idx) const = 0;
  virtual uint8_t GetState() const = 0;
  virtual uint16_t GetGdbPort() const = 0;
  virtual uint64_t GetCycle() const = 0;
  virtual uint32_t GetProgramCounter() const = 0;
  virtual uint16_t GetStackPointer() const = 0;
  virtual uint16_t GetBssSectionSize() const = 0;
  virtual uint16_t GetDataSectionSize() const = 0;
  virtual uint16_t GetRamSize() const = 0;
  virtual uint32_t TwiIrqMsg(uint8_t msg, uint8_t addr, uint8_t data) const = 0;
};
}  // namespace simulator
}  // namespace threeboard
