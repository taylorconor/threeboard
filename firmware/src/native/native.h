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

// PSTR is only defined by avr-libc, this just guarantees that it's always
// defined (although may not do anything if PROGMEM is a noop).
#undef PSTR
#define PSTR(s) ((const PROGMEM char *)(s))

namespace threeboard {
namespace native {

class Native {
 public:
  virtual ~Native() = default;

  virtual TimerInterruptHandlerDelegate *GetTimerInterruptHandlerDelegate()
      const = 0;
  virtual void SetTimerInterruptHandlerDelegate(
      TimerInterruptHandlerDelegate *) = 0;
  virtual UsbInterruptHandlerDelegate *GetUsbInterruptHandlerDelegate()
      const = 0;
  virtual void SetUsbInterruptHandlerDelegate(
      UsbInterruptHandlerDelegate *) = 0;

  virtual void EnableInterrupts() = 0;
  virtual void DisableInterrupts() = 0;
  virtual void EnableCpuSleep() = 0;
  virtual void SleepCpu() = 0;
  virtual void DisableCpuSleep() = 0;

  virtual void EnableTimer1() = 0;
  virtual void EnableTimer3() = 0;

  virtual void DelayMs(uint8_t) = 0;

  virtual uint16_t ReadPgmWord(const uint8_t *) const = 0;
  virtual uint8_t ReadPgmByte(const uint8_t *) const = 0;

  virtual void EepromReadByte(const uint16_t &, uint8_t *) const = 0;
  virtual void EepromWriteByte(const uint16_t &, uint8_t) = 0;

  virtual void EnableDDRB(uint8_t) = 0;
  virtual void DisableDDRB(uint8_t) = 0;
  virtual void EnableDDRC(uint8_t) = 0;
  virtual void EnableDDRD(uint8_t) = 0;
  virtual void EnableDDRF(uint8_t) = 0;

  virtual void EnablePORTB(uint8_t) = 0;
  virtual void DisablePORTB(uint8_t) = 0;
  virtual void EnablePORTC(uint8_t) = 0;
  virtual void DisablePORTC(uint8_t) = 0;
  virtual void EnablePORTD(uint8_t) = 0;
  virtual void DisablePORTD(uint8_t) = 0;
  virtual void EnablePORTF(uint8_t) = 0;
  virtual void DisablePORTF(uint8_t) = 0;

  virtual uint8_t GetPINB() const = 0;

  // TODO: order these
  virtual void SetUEDATX(uint8_t) = 0;
  virtual uint8_t GetUEDATX() = 0;
  virtual void SetUEINTX(uint8_t) = 0;
  virtual uint8_t GetUEINTX() const = 0;
  virtual void SetUDINT(uint8_t) = 0;
  virtual uint8_t GetUDINT() const = 0;
  virtual uint8_t GetRXOUTI() const = 0;
  virtual uint8_t GetEORSTI() const = 0;
  virtual uint8_t GetEPEN() const = 0;
  virtual void SetUECONX(uint8_t) = 0;
  virtual uint8_t GetUECFG0X() const = 0;
  virtual void SetUECFG1X(uint8_t) = 0;
  virtual uint8_t GetUDMFN() const = 0;
  virtual uint8_t GetSREG() const = 0;
  virtual void SetSREG(uint8_t) = 0;
  virtual uint8_t GetUDFNUML() const = 0;
  virtual void SetUHWCON(uint8_t) = 0;
  virtual void SetUSBCON(uint8_t) = 0;
  virtual void SetPLLCSR(uint8_t) = 0;
  virtual uint8_t GetPLLCSR() const = 0;
  virtual void SetUENUM(uint8_t) = 0;
  virtual void SetUDCON(uint8_t) = 0;
  virtual void SetUDIEN(uint8_t) = 0;
  virtual uint8_t GetUDCON() const = 0;
  virtual void SetUECFG0X(uint8_t) = 0;
  virtual void SetUEIENX(uint8_t) = 0;
  virtual uint8_t GetUDADDR() const = 0;
  virtual void SetUDADDR(uint8_t) = 0;
  virtual void SetUERST(uint8_t) = 0;

  // Two wire registers
  virtual void SetTWAMR(uint8_t) = 0;
  virtual void SetTWSR(uint8_t) = 0;
  virtual uint8_t GetTWSR() const = 0;
  virtual void SetTWBR(uint8_t) = 0;
  virtual void SetTWCR(uint8_t) = 0;
  virtual uint8_t GetTWCR() const = 0;
  virtual void SetTWDR(uint8_t) = 0;
  virtual uint8_t GetTWDR() const = 0;

  virtual volatile uint8_t &GetUCSR1A() const = 0;
  virtual void SetUCSR1A(uint8_t) = 0;
  virtual volatile uint8_t &GetUCSR1B() const = 0;
  virtual void SetUCSR1B(uint8_t) = 0;
  virtual volatile uint8_t &GetUCSR1C() const = 0;
  virtual void SetUCSR1C(uint8_t) = 0;
  virtual void SetUDR1(uint8_t) = 0;
  virtual void SetUBRR1H(uint8_t) = 0;
  virtual void SetUBRR1L(uint8_t) = 0;
};

typedef void (Native::*PortModFn)(uint8_t);

}  // namespace native
}  // namespace threeboard
