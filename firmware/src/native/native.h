#pragma once

#include <stdint.h>

#include "src/native/timer_interrupt_handler_delegate.h"
#include "src/usb/usb_interrupt_handler_delegate.h"

#undef PROGMEM
#ifdef __AVR__
#define PROGMEM __attribute__((__progmem__))
#else
#define PROGMEM
#endif

namespace threeboard {
namespace native {

// Define port and register offset definitions for the atmega32u4 here so we can
// compile them into x86 code without having to link against the avr library.
// These are microcontroller-specific! They can be found in <avr/iom32u4.h>.

// Port B
constexpr uint8_t PB0 = 0;
constexpr uint8_t PB1 = 1;
constexpr uint8_t PB2 = 2;
constexpr uint8_t PB3 = 3;
constexpr uint8_t PB4 = 4;
constexpr uint8_t PB5 = 5;
constexpr uint8_t PB6 = 6;
constexpr uint8_t PB7 = 7;

// Port D
constexpr uint8_t PD0 = 0;
constexpr uint8_t PD1 = 1;
constexpr uint8_t PD2 = 2;
constexpr uint8_t PD3 = 3;
constexpr uint8_t PD4 = 4;
constexpr uint8_t PD5 = 5;
constexpr uint8_t PD6 = 6;
constexpr uint8_t PD7 = 7;

// Port F
constexpr uint8_t PF0 = 0;
constexpr uint8_t PF1 = 1;
constexpr uint8_t PF4 = 4;
constexpr uint8_t PF5 = 5;
constexpr uint8_t PF6 = 6;
constexpr uint8_t PF7 = 7;

// UEINTX
constexpr uint8_t TXINI = 0;
constexpr uint8_t RXOUTI = 2;
constexpr uint8_t RXSTPI = 3;
constexpr uint8_t RWAL = 5;

// UECONX
constexpr uint8_t EPEN = 0;
constexpr uint8_t STALLRQ = 5;

// UDIEN
constexpr uint8_t SOFE = 2;
constexpr uint8_t EORSTE = 3;

// UEIENX
constexpr uint8_t RXSTPE = 3;

// UDINT
constexpr uint8_t SOFI = 2;
constexpr uint8_t EORSTI = 3;

// UDMFN
constexpr uint8_t FNCERR = 4;

// UHWCON
constexpr uint8_t UVREGE = 0;

// USBCON
constexpr uint8_t OTGPADE = 4;
constexpr uint8_t FRZCLK = 5;
constexpr uint8_t USBE = 7;

// UDCON
constexpr uint8_t DETACH = 0;

// PLLCSR
constexpr uint8_t PLOCK = 0;
constexpr uint8_t PLLE = 1;
constexpr uint8_t PINDIV = 4;

// UDADDR
constexpr uint8_t ADDEN = 7;

class Native {
public:
  virtual ~Native() {}

  virtual TimerInterruptHandlerDelegate *
  GetTimerInterruptHandlerDelegate() const = 0;
  virtual void
  SetTimerInterruptHandlerDelegate(TimerInterruptHandlerDelegate *) = 0;
  virtual usb::UsbInterruptHandlerDelegate *
  GetUsbInterruptHandlerDelegate() const = 0;
  virtual void
  SetUsbInterruptHandlerDelegate(usb::UsbInterruptHandlerDelegate *) = 0;

  virtual void EnableInterrupts() = 0;
  virtual void DisableInterrupts() = 0;

  virtual void EnableTimer1() = 0;
  virtual void EnableTimer3() = 0;

  virtual void Delay(uint8_t) const = 0;

  virtual uint16_t ReadPgmWord(const uint8_t *) = 0;
  virtual uint8_t ReadPgmByte(const uint8_t *) = 0;

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
  virtual uint8_t GetUEINTX() = 0;
  virtual void SetUDINT(const uint8_t) = 0;
  virtual uint8_t GetUDINT() = 0;
  virtual uint8_t GetRXOUTI() = 0;
  virtual uint8_t GetEORSTI() = 0;
  virtual uint8_t GetEPEN() = 0;
  virtual void SetUECONX(const uint8_t) = 0;
  virtual uint8_t GetUECFG0X() = 0;
  virtual void SetUECFG1X(const uint8_t) = 0;
  virtual uint8_t GetUDMFN() = 0;
  virtual uint8_t GetSREG() = 0;
  virtual void SetSREG(const uint8_t) = 0;
  virtual uint8_t GetUDFNUML() = 0;
  virtual void SetUHWCON(const uint8_t) = 0;
  virtual void SetUSBCON(const uint8_t) = 0;
  virtual void SetPLLCSR(const uint8_t) = 0;
  virtual uint8_t GetPLLCSR() = 0;
  virtual void SetUENUM(const uint8_t) = 0;
  virtual void SetUDCON(const uint8_t) = 0;
  virtual void SetUDIEN(const uint8_t) = 0;
  virtual uint8_t GetUDCON() = 0;
  virtual void SetUECFG0X(const uint8_t) = 0;
  virtual void SetUEIENX(const uint8_t) = 0;
  virtual uint8_t GetUDADDR() = 0;
  virtual void SetUDADDR(const uint8_t) = 0;
  virtual void SetUERST(const uint8_t) = 0;
};

} // namespace native
} // namespace threeboard
