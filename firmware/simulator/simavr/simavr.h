#pragma once

#include <stdint.h>

#include <functional>
#include <memory>

#include "simulator/simavr/internal/iom32u4.h"
#include "simulator/util/lifetime.h"

// TODO: Remove asap!
extern "C" {
struct avr_t;
}

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

enum Flag {
  UART_FLAG_STDIO = 2,
};

// Define all necessary IOCTL indices here so we don't have to depend on any
// simavr headers. The strange looking constants are an artifact of the very
// weird way that simavr identifies IOCTL indices.
enum Ioctl {
  // The USB ioctls are defined in avr_usb.h.
  USB_WRITE = 1970496119,
  USB_READ = 1970496114,
  USB_SETUP = 1970496115,
  USB_RESET = 1970496082,
  USB_VBUS = 1970496086,

  // The UART ioctls are defined in avr_uart.c.
  UART_GET_FLAGS = 1969317681,
  UART_SET_FLAGS = 1969320753,
};

// A copy of the avr_io_usb, defined in avr_usb.h.
struct UsbPacketBuffer {
  uint8_t endpoint;
  uint32_t size;
  uint8_t *buffer;
};

// A copy of avr_twi_msg_t, defined in avr_twi.h.
typedef struct TwiMessage {
  uint32_t unused : 8, msg : 8, addr : 8, data : 8;
} TwiMessage;

// A copy of avr_twi_msg_irq_t, defined in avr_twi.h.
typedef struct TwiMessageIrq {
  union {
    uint32_t v;
    TwiMessage twi;
  } u;
} TwiMessageIrq;

// A copy of the anonymous enum defined in avr_twi.h.
enum { TWI_IRQ_INPUT = 0, TWI_IRQ_OUTPUT, TWI_IRQ_STATUS, TWI_IRQ_COUNT };

// A copy of the anonymous enum defined in avr_twi.h.
enum {
  TWI_COND_START = (1 << 0),
  TWI_COND_STOP = (1 << 1),
  TWI_COND_ADDR = (1 << 2),
  TWI_COND_ACK = (1 << 3),
  TWI_COND_WRITE = (1 << 4),
  TWI_COND_READ = (1 << 5),
};

using UsbAttachCallback = std::function<void(uint32_t)>;
using UartOutputCallback = std::function<void(uint8_t)>;
using I2cMessageCallback = std::function<void(uint32_t)>;

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

  virtual void RaiseI2cIrq(uint8_t direction, uint32_t value) = 0;

  virtual void SetData(uint8_t idx, uint8_t val) = 0;
  virtual void SetState(uint8_t val) = 0;
  virtual void SetGdbPort(uint8_t val) = 0;

  virtual uint8_t GetData(uint8_t idx) const = 0;
  virtual uint8_t GetState() const = 0;
  virtual uint8_t GetGdbPort() const = 0;
  virtual uint64_t GetCycle() const = 0;
  virtual uint16_t GetStackPointer() const = 0;
  virtual uint16_t GetBssSectionSize() const = 0;
  virtual uint16_t GetDataSectionSize() const = 0;
  virtual uint16_t GetRamSize() const = 0;

  virtual uint32_t TwiIrqMsg(uint8_t msg, uint8_t addr, uint8_t data) const = 0;
};
}  // namespace simulator
}  // namespace threeboard
