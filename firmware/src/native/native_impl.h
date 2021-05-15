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

class NativeImpl final : public Native {
 public:
  NativeImpl();
  ~NativeImpl() override = default;

  TimerInterruptHandlerDelegate *GetTimerInterruptHandlerDelegate()
      const override;
  void SetTimerInterruptHandlerDelegate(
      TimerInterruptHandlerDelegate *) override;
  UsbInterruptHandlerDelegate *GetUsbInterruptHandlerDelegate() const override;
  void SetUsbInterruptHandlerDelegate(UsbInterruptHandlerDelegate *) override;

  void EnableInterrupts() override;
  void DisableInterrupts() override;
  void EnableCpuSleep() override;
  void SleepCpu() override;
  void DisableCpuSleep() override;

  void EnableTimer1() override;
  void EnableTimer3() override;

  void DelayMs(uint8_t) override;

  uint16_t ReadPgmWord(const uint8_t *) const override;
  uint8_t ReadPgmByte(const uint8_t *) const override;

  void EepromWrite(const uint16_t &, uint8_t *, const uint16_t &) override;
  void EepromRead(const uint16_t &, uint8_t *, const uint16_t &) const override;

  void EnableDDRB(uint8_t) override;
  void DisableDDRB(uint8_t) override;
  void EnableDDRC(uint8_t) override;
  void EnableDDRD(uint8_t) override;
  void EnableDDRF(uint8_t) override;

  void EnablePORTB(uint8_t) override;
  void DisablePORTB(uint8_t) override;
  void EnablePORTC(uint8_t) override;
  void DisablePORTC(uint8_t) override;
  void EnablePORTD(uint8_t) override;
  void DisablePORTD(uint8_t) override;
  void EnablePORTF(uint8_t) override;
  void DisablePORTF(uint8_t) override;

  uint8_t GetPINB() const override;

  void SetUEDATX(uint8_t) override;
  uint8_t GetUEDATX() override;
  void SetUEINTX(uint8_t) override;
  uint8_t GetUEINTX() const override;
  void SetUDINT(uint8_t) override;
  uint8_t GetUDINT() const override;
  uint8_t GetRXOUTI() const override;
  uint8_t GetEORSTI() const override;
  uint8_t GetEPEN() const override;
  void SetUECONX(uint8_t) override;
  uint8_t GetUECFG0X() const override;
  void SetUECFG1X(uint8_t) override;
  uint8_t GetUDMFN() const override;
  uint8_t GetSREG() const override;
  void SetSREG(uint8_t) override;
  uint8_t GetUDFNUML() const override;
  void SetUHWCON(uint8_t) override;
  void SetUSBCON(uint8_t) override;
  void SetPLLCSR(uint8_t) override;
  uint8_t GetPLLCSR() const override;
  void SetUENUM(uint8_t) override;
  void SetUDCON(uint8_t) override;
  void SetUDIEN(uint8_t) override;
  uint8_t GetUDCON() const override;
  void SetUECFG0X(uint8_t) override;
  void SetUEIENX(uint8_t) override;
  uint8_t GetUDADDR() const override;
  void SetUDADDR(uint8_t) override;
  void SetUERST(uint8_t) override;

  void SetTWAMR(uint8_t) override;
  uint8_t GetTWSR() const override;
  void SetTWSR(uint8_t) override;
  void SetTWBR(uint8_t) override;
  uint8_t GetTWCR() const override;
  void SetTWCR(uint8_t) override;
  uint8_t GetTWDR() const override;
  void SetTWDR(uint8_t) override;

  volatile uint8_t &GetUCSR1A() const override;
  void SetUCSR1A(uint8_t) override;
  volatile uint8_t &GetUCSR1B() const override;
  void SetUCSR1B(uint8_t) override;
  volatile uint8_t &GetUCSR1C() const override;
  void SetUCSR1C(uint8_t) override;
  void SetUDR1(uint8_t) override;
  void SetUBRR1H(uint8_t) override;
  void SetUBRR1L(uint8_t) override;

 private:
  TimerInterruptHandlerDelegate *timer_delegate_;
  UsbInterruptHandlerDelegate *usb_delegate_;
};

}  // namespace native
}  // namespace threeboard
