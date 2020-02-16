#pragma once

#include "native.h"

#include "src/usb/usb_interrupt_handler_delegate.h"

// The threeboard uses an atmega32u4 microcontroller with a 16MHz clock.
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Provide an implementation of __cxa_guard* functions to allow the AVR compiler
// to link against guards for static variable initialisation (which are not
// provided otherwise). x86 compilation will link against these by default.
__extension__ typedef int __guard __attribute__((mode(__DI__)));
extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release(__guard *);
extern "C" void __cxa_guard_abort(__guard *);

namespace threeboard {
namespace native {

class NativeImpl : public Native {
public:
  static Native *Get();
  ~NativeImpl() {}

  TimerInterruptHandlerDelegate *
  GetTimerInterruptHandlerDelegate() const final override;
  void SetTimerInterruptHandlerDelegate(
      TimerInterruptHandlerDelegate *) final override;
  usb::UsbInterruptHandlerDelegate *
  GetUsbInterruptHandlerDelegate() const final override;
  void SetUsbInterruptHandlerDelegate(
      usb::UsbInterruptHandlerDelegate *) final override;

  void EnableInterrupts() final override;
  void DisableInterrupts() final override;

  void EnableTimer1() final override;
  void EnableTimer3() final override;

  void Delay(uint8_t) const final override;

  uint16_t ReadPgmWord(const uint8_t *) final override;
  uint8_t ReadPgmByte(const uint8_t *) final override;

  void EnableDDRB(const uint8_t) final override;
  void DisableDDRB(const uint8_t) final override;
  void EnableDDRD(const uint8_t) final override;
  void EnableDDRF(const uint8_t) final override;

  void EnablePORTB(const uint8_t) final override;
  void DisablePORTB(const uint8_t) final override;
  void EnablePORTD(const uint8_t) final override;
  void DisablePORTD(const uint8_t) final override;
  void EnablePORTF(const uint8_t) final override;
  void DisablePORTF(const uint8_t) final override;

  uint8_t GetPINB() const final override;

  void SetUEDATX(const uint8_t) final override;
  uint8_t GetUEDATX() final override;
  void SetUEINTX(const uint8_t) final override;
  uint8_t GetUEINTX() final override;
  void SetUDINT(const uint8_t) final override;
  uint8_t GetUDINT() final override;
  uint8_t GetRXOUTI() final override;
  uint8_t GetEORSTI() final override;
  uint8_t GetEPEN() final override;
  void SetUECONX(const uint8_t) final override;
  uint8_t GetUECFG0X() final override;
  void SetUECFG1X(const uint8_t) final override;
  uint8_t GetUDMFN() final override;
  uint8_t GetSREG() final override;
  void SetSREG(const uint8_t) final override;
  uint8_t GetUDFNUML() final override;
  void SetUHWCON(const uint8_t) final override;
  void SetUSBCON(const uint8_t) final override;
  void SetPLLCSR(const uint8_t) final override;
  uint8_t GetPLLCSR() final override;
  void SetUENUM(const uint8_t) final override;
  void SetUDCON(const uint8_t) final override;
  void SetUDIEN(const uint8_t) final override;
  uint8_t GetUDCON() final override;
  void SetUECFG0X(const uint8_t) final override;
  void SetUEIENX(const uint8_t) final override;
  uint8_t GetUDADDR() final override;
  void SetUDADDR(const uint8_t) final override;
  void SetUERST(const uint8_t) final override;

private:
  NativeImpl() = default;

  TimerInterruptHandlerDelegate *timer_delegate_;
  usb::UsbInterruptHandlerDelegate *usb_delegate_;
};

} // namespace native
} // namespace threeboard
