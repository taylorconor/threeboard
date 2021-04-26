#pragma once

#include "src/delegates/usb_interrupt_handler_delegate.h"
#include "src/native/native.h"

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
  ~NativeImpl() override = default;

  TimerInterruptHandlerDelegate *GetTimerInterruptHandlerDelegate() const final;
  void SetTimerInterruptHandlerDelegate(TimerInterruptHandlerDelegate *) final;
  UsbInterruptHandlerDelegate *GetUsbInterruptHandlerDelegate() const final;
  void SetUsbInterruptHandlerDelegate(UsbInterruptHandlerDelegate *) final;

  void EnableInterrupts() final;
  void DisableInterrupts() final;
  void EnableCpuSleep() final;
  void SleepCpu() final;
  void DisableCpuSleep() final;

  void EnableTimer1() final;
  void EnableTimer3() final;

  void DelayMs(uint8_t) final;

  uint16_t ReadPgmWord(const uint8_t *) const final;
  uint8_t ReadPgmByte(const uint8_t *) const final;

  virtual void EepromWrite(const uint16_t &, uint8_t *, const uint16_t &) final;
  virtual void EepromRead(const uint16_t &, uint8_t *,
                          const uint16_t &) const final;

  void EnableDDRB(uint8_t) final;
  void DisableDDRB(uint8_t) final;
  void EnableDDRC(uint8_t) final;
  void EnableDDRD(uint8_t) final;
  void EnableDDRF(uint8_t) final;

  void EnablePORTB(uint8_t) final;
  void DisablePORTB(uint8_t) final;
  void EnablePORTC(uint8_t) final;
  void DisablePORTC(uint8_t) final;
  void EnablePORTD(uint8_t) final;
  void DisablePORTD(uint8_t) final;
  void EnablePORTF(uint8_t) final;
  void DisablePORTF(uint8_t) final;

  uint8_t GetPINB() const final;

  void SetUEDATX(uint8_t) final;
  uint8_t GetUEDATX() final;
  void SetUEINTX(uint8_t) final;
  uint8_t GetUEINTX() const final;
  void SetUDINT(uint8_t) final;
  uint8_t GetUDINT() const final;
  uint8_t GetRXOUTI() const final;
  uint8_t GetEORSTI() const final;
  uint8_t GetEPEN() const final;
  void SetUECONX(uint8_t) final;
  uint8_t GetUECFG0X() const final;
  void SetUECFG1X(uint8_t) final;
  uint8_t GetUDMFN() const final;
  uint8_t GetSREG() const final;
  void SetSREG(uint8_t) final;
  uint8_t GetUDFNUML() const final;
  void SetUHWCON(uint8_t) final;
  void SetUSBCON(uint8_t) final;
  void SetPLLCSR(uint8_t) final;
  uint8_t GetPLLCSR() const final;
  void SetUENUM(uint8_t) final;
  void SetUDCON(uint8_t) final;
  void SetUDIEN(uint8_t) final;
  uint8_t GetUDCON() const final;
  void SetUECFG0X(uint8_t) final;
  void SetUEIENX(uint8_t) final;
  uint8_t GetUDADDR() const final;
  void SetUDADDR(uint8_t) final;
  void SetUERST(uint8_t) final;

  void SetTWAMR(uint8_t) final;
  uint8_t GetTWSR() const final;
  void SetTWSR(uint8_t) final;
  void SetTWBR(uint8_t) final;
  uint8_t GetTWCR() const final;
  void SetTWCR(uint8_t) final;
  uint8_t GetTWDR() const final;
  void SetTWDR(uint8_t) final;

  volatile uint8_t &GetUCSR1A() const final;
  void SetUCSR1A(uint8_t) final;
  volatile uint8_t &GetUCSR1B() const final;
  void SetUCSR1B(uint8_t) final;
  volatile uint8_t &GetUCSR1C() const final;
  void SetUCSR1C(uint8_t) final;
  void SetUDR1(uint8_t) final;
  void SetUBRR1H(uint8_t) final;
  void SetUBRR1L(uint8_t) final;

 private:
  NativeImpl() = default;

  TimerInterruptHandlerDelegate *timer_delegate_;
  UsbInterruptHandlerDelegate *usb_delegate_;
};

}  // namespace native
}  // namespace threeboard
