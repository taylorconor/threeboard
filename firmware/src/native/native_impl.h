#pragma once

#include "native.h"

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

  void SetTimer1InterruptHandler(InterruptHandlerDelegate *) override;
  void SetTimer3InterruptHandler(InterruptHandlerDelegate *) override;
  InterruptHandlerDelegate *GetInterruptHandlerDelegate() const override;

  void Delay(uint8_t) const override;

  void EnableDDRB(const uint8_t) override;
  void DisableDDRB(const uint8_t) override;
  void EnableDDRD(const uint8_t) override;
  void EnableDDRF(const uint8_t) override;

  void EnablePORTB(const uint8_t) override;
  void DisablePORTB(const uint8_t) override;
  void EnablePORTD(const uint8_t) override;
  void DisablePORTD(const uint8_t) override;
  void EnablePORTF(const uint8_t) override;
  void DisablePORTF(const uint8_t) override;

  uint8_t GetPINB() const override;

private:
  NativeImpl() = default;

  InterruptHandlerDelegate *delegate_;
};

} // namespace native
} // namespace threeboard
