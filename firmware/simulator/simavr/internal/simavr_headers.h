#pragma once

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
struct TwiMessage {
  uint32_t unused : 8, msg : 8, addr : 8, data : 8;
};

// A copy of avr_twi_msg_irq_t, defined in avr_twi.h.
struct TwiMessageIrq {
  union {
    uint32_t v;
    TwiMessage twi;
  } u;
};

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

}  // namespace simulator
}  // namespace threeboard