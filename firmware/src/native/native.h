#pragma once

#include <stdint.h>

namespace threeboard {
namespace native {

// Define port definitions for the atmega32u4 here so we can compile them into
// x86 code without having to link against the avr library. These are
// microcontroller-specific! They can be found in <avr/iom32u4.h>
constexpr uint8_t PB0 = 0;
constexpr uint8_t PB1 = 1;
constexpr uint8_t PB2 = 2;
constexpr uint8_t PB3 = 3;
constexpr uint8_t PB4 = 4;
constexpr uint8_t PB5 = 5;
constexpr uint8_t PB6 = 6;
constexpr uint8_t PB7 = 7;

constexpr uint8_t PD0 = 0;
constexpr uint8_t PD1 = 1;
constexpr uint8_t PD2 = 2;
constexpr uint8_t PD3 = 3;
constexpr uint8_t PD4 = 4;
constexpr uint8_t PD5 = 5;
constexpr uint8_t PD6 = 6;
constexpr uint8_t PD7 = 7;

constexpr uint8_t PF0 = 0;
constexpr uint8_t PF1 = 1;
constexpr uint8_t PF4 = 4;
constexpr uint8_t PF5 = 5;
constexpr uint8_t PF6 = 6;
constexpr uint8_t PF7 = 7;

#define __always_inline inline __attribute__((__always_inline__))

class InterruptHandlerDelegate {
public:
  virtual void HandleTimer1Interrupt() = 0;
  virtual void HandleTimer3Interrupt() = 0;
};

class Native {
public:
  virtual ~Native() {}

  virtual void SetTimer1InterruptHandler(InterruptHandlerDelegate *) = 0;
  virtual void SetTimer3InterruptHandler(InterruptHandlerDelegate *) = 0;
  virtual InterruptHandlerDelegate *GetInterruptHandlerDelegate() const = 0;

  virtual void Delay(uint8_t) const = 0;

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
};

} // namespace native
} // namespace threeboard
