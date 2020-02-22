#pragma once

#include <stdint.h>

#include "src/delegates/timer_interrupt_handler_delegate.h"
#include "src/delegates/usb_interrupt_handler_delegate.h"
#include "src/native/constants.h"

#undef PROGMEM
#ifdef __AVR__
// Only define PROGMEM for avr-gcc since it's not supported (or needed) when
// compiling for x86.
#define PROGMEM __attribute__((__progmem__))
#else
// Avoid noisy compiler warnings when compiling for x86.
#define PROGMEM
#endif

namespace threeboard {
namespace native {

class Native {
public:
  virtual ~Native() {}

  virtual TimerInterruptHandlerDelegate *
  GetTimerInterruptHandlerDelegate() const = 0;
  virtual void
  SetTimerInterruptHandlerDelegate(TimerInterruptHandlerDelegate *) = 0;
  virtual UsbInterruptHandlerDelegate *
  GetUsbInterruptHandlerDelegate() const = 0;
  virtual void
  SetUsbInterruptHandlerDelegate(UsbInterruptHandlerDelegate *) = 0;

  virtual void EnableInterrupts() = 0;
  virtual void DisableInterrupts() = 0;

  virtual void EnableTimer1() = 0;
  virtual void EnableTimer3() = 0;

  virtual void Delay(const uint8_t) const = 0;

  virtual uint16_t ReadPgmWord(const uint8_t *) const = 0;
  virtual uint8_t ReadPgmByte(const uint8_t *) const = 0;

  virtual void EnableDDRB(const uint8_t) = 0;
  virtual void DisableDDRB(const uint8_t) = 0;
  virtual void EnableDDRD(const uint8_t) = 0;
  virtual void EnableDDRF(const uint8_t) = 0;

  virtual void EnablePORTB(const uint8_t) = 0;
  virtual void DisablePORTB(const uint8_t) = 0;
  virtual void EnablePORTD(const uint8_t) = 0;
  virtual void DisablePORTD(const uint8_t) = 0;
  virtual void EnablePORTF(const uint8_t) = 0;
  virtual void DisablePORTF(const uint8_t) = 0;

  virtual uint8_t GetPINB() const = 0;

  // TODO: order these
  virtual void SetUEDATX(const uint8_t) = 0;
  virtual uint8_t GetUEDATX() = 0;
  virtual void SetUEINTX(const uint8_t) = 0;
  virtual uint8_t GetUEINTX() const = 0;
  virtual void SetUDINT(const uint8_t) = 0;
  virtual uint8_t GetUDINT() const = 0;
  virtual uint8_t GetRXOUTI() const = 0;
  virtual uint8_t GetEORSTI() const = 0;
  virtual uint8_t GetEPEN() const = 0;
  virtual void SetUECONX(const uint8_t) = 0;
  virtual uint8_t GetUECFG0X() const = 0;
  virtual void SetUECFG1X(const uint8_t) = 0;
  virtual uint8_t GetUDMFN() const = 0;
  virtual uint8_t GetSREG() const = 0;
  virtual void SetSREG(const uint8_t) = 0;
  virtual uint8_t GetUDFNUML() const = 0;
  virtual void SetUHWCON(const uint8_t) = 0;
  virtual void SetUSBCON(const uint8_t) = 0;
  virtual void SetPLLCSR(const uint8_t) = 0;
  virtual uint8_t GetPLLCSR() const = 0;
  virtual void SetUENUM(const uint8_t) = 0;
  virtual void SetUDCON(const uint8_t) = 0;
  virtual void SetUDIEN(const uint8_t) = 0;
  virtual uint8_t GetUDCON() const = 0;
  virtual void SetUECFG0X(const uint8_t) = 0;
  virtual void SetUEIENX(const uint8_t) = 0;
  virtual uint8_t GetUDADDR() const = 0;
  virtual void SetUDADDR(const uint8_t) = 0;
  virtual void SetUERST(const uint8_t) = 0;
};

} // namespace native
} // namespace threeboard
