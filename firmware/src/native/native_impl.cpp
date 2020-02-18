#include "native_impl.h"

// Defining this allows delay_ms to accept non-compiletime constants as
// parameters.
#define __DELAY_BACKWARD_COMPATIBLE__

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/delay.h>

int __cxa_guard_acquire(__guard *g) { return !*(char *)(g); }
void __cxa_guard_release(__guard *g) { *(char *)g = 1; }
void __cxa_guard_abort(__guard *) {}

// The delete operator needs to be implemented, since even generating code for
// the virtual destructor of Native requires linking against a delete operator.
// This isn't a problem for the Native class because it's a singleton, so in
// practice it has no effect.
void operator delete(void *ptr, unsigned int size) { free(ptr); }

namespace threeboard {
namespace native {
namespace {

void Timer1Init() {
  // Enable timer 1 in CTC mode (clear timer on compare), with prescaler set to
  // 1 (so using native 16MHz clock frequency).
  TCCR1B |= (1 << WGM12) | (1 << CS10);

  // Set timer1's initial value to 0.
  TCNT1 = 0;

  // Set the compare value. We want the interrupt to fire once every 2ms to
  // avoid LED flicker and missed keystrokes; The 16MHz clock ticks every
  // 0.0000625ms, so we calculate the compare value as 2ms/0.0000625ms =
  // 32000. Subtract 1 because the timer starts from 0.
  OCR1A = 31999;

  // Enable output compare interrupts for timer 1, using compare value A
  // (OCIE1A).
  TIMSK1 |= (1 << OCIE1A);
}

void Timer3Init() {
  // Enable timer 3 in CTC mode, with prescaler set to 8.
  TCCR3B |= (1 << WGM32) | (1 << CS31);

  TCNT3 = 0;

  // Set the compare value. We want the interrupt to fire once every 5ms, since
  // the debounce code scans key states in 5ms increments. (5ms/0.0000625ms)/8 =
  // 10000.
  OCR3A = 10000;

  TIMSK3 |= (1 << OCIE3A);
}

// Define the interrupt service registers (ISRs) for timers 1 and 3 (the
// atmega32u4 has no timer2, and timer0 is used for the system clock). The
// interrupt handlers are guaranteed to be defined because individual timer
// interrupts are enabled in the SetTimerNInterruptHandler methods which set the
// handler before enabling interrupts for that timer.
ISR(TIMER1_COMPA_vect) {
  NativeImpl::Get()
      ->GetTimerInterruptHandlerDelegate()
      ->HandleTimer1Interrupt();
}
ISR(TIMER3_COMPA_vect) {
  NativeImpl::Get()
      ->GetTimerInterruptHandlerDelegate()
      ->HandleTimer3Interrupt();
}

// ISR for USB general interrupts.
ISR(USB_GEN_vect) {
  NativeImpl::Get()->GetUsbInterruptHandlerDelegate()->HandleGeneralInterrupt();
}

// ISR for USB endpoint interrupts.
ISR(USB_COM_vect) {
  NativeImpl::Get()
      ->GetUsbInterruptHandlerDelegate()
      ->HandleEndpointInterrupt();
}
} // namespace

Native *NativeImpl::Get() {
  static NativeImpl instance = NativeImpl();
  return &instance;
}

TimerInterruptHandlerDelegate *
NativeImpl::GetTimerInterruptHandlerDelegate() const {
  return timer_delegate_;
}

void NativeImpl::SetTimerInterruptHandlerDelegate(
    TimerInterruptHandlerDelegate *delegate) {
  timer_delegate_ = delegate;
}

usb::UsbInterruptHandlerDelegate *
NativeImpl::GetUsbInterruptHandlerDelegate() const {
  return usb_delegate_;
}

void NativeImpl::SetUsbInterruptHandlerDelegate(
    usb::UsbInterruptHandlerDelegate *delegate) {
  usb_delegate_ = delegate;
}

void NativeImpl::EnableInterrupts() { sei(); }

void NativeImpl::DisableInterrupts() { cli(); }

void NativeImpl::EnableTimer1() { Timer1Init(); }

void NativeImpl::EnableTimer3() { Timer3Init(); }

void NativeImpl::Delay(const uint8_t ms) const { _delay_ms(ms); }

uint16_t NativeImpl::ReadPgmWord(const uint8_t *ptr) const {
  return pgm_read_word(ptr);
}

uint8_t NativeImpl::ReadPgmByte(const uint8_t *ptr) const {
  return pgm_read_byte(ptr);
}

void NativeImpl::EnableDDRB(const uint8_t val) { DDRB |= val; }
void NativeImpl::DisableDDRB(const uint8_t val) { DDRB &= ~val; }
void NativeImpl::EnableDDRD(const uint8_t val) { DDRD |= val; }
void NativeImpl::EnableDDRF(const uint8_t val) { DDRF |= val; }

void NativeImpl::EnablePORTB(const uint8_t val) { PORTB |= val; }
void NativeImpl::DisablePORTB(const uint8_t val) { PORTB &= ~val; }
void NativeImpl::EnablePORTD(const uint8_t val) { PORTD |= val; }
void NativeImpl::DisablePORTD(const uint8_t val) { PORTD &= ~val; }
void NativeImpl::EnablePORTF(const uint8_t val) { PORTF |= val; }
void NativeImpl::DisablePORTF(const uint8_t val) { PORTF &= ~val; }

uint8_t NativeImpl::GetPINB() const { return PINB; }

void NativeImpl::SetUEDATX(const uint8_t val) { UEDATX = val; }
uint8_t NativeImpl::GetUEDATX() { return UEDATX; }
void NativeImpl::SetUEINTX(const uint8_t val) { UEINTX = val; }
uint8_t NativeImpl::GetUEINTX() const { return UEINTX; }
void NativeImpl::SetUDINT(const uint8_t val) { UDINT = val; }
uint8_t NativeImpl::GetUDINT() const { return UDINT; }

uint8_t NativeImpl::GetRXOUTI() const { return RXOUTI; }
uint8_t NativeImpl::GetEORSTI() const { return EORSTI; }
uint8_t NativeImpl::GetEPEN() const { return EPEN; }
void NativeImpl::SetUECONX(const uint8_t val) { UECONX = val; }
uint8_t NativeImpl::GetUECFG0X() const { return UECFG0X; }
void NativeImpl::SetUECFG1X(const uint8_t val) { UECFG1X = val; }
uint8_t NativeImpl::GetUDMFN() const { return UDMFN; }
uint8_t NativeImpl::GetSREG() const { return SREG; }
void NativeImpl::SetSREG(const uint8_t val) { SREG = val; }
uint8_t NativeImpl::GetUDFNUML() const { return UDFNUML; }
void NativeImpl::SetUHWCON(const uint8_t val) { UHWCON = val; }
void NativeImpl::SetUSBCON(const uint8_t val) { USBCON = val; }

void NativeImpl::SetPLLCSR(const uint8_t val) { PLLCSR = val; }
uint8_t NativeImpl::GetPLLCSR() const { return PLLCSR; }
void NativeImpl::SetUENUM(const uint8_t val) { UENUM = val; }
void NativeImpl::SetUDCON(const uint8_t val) { UDCON = val; }
void NativeImpl::SetUDIEN(const uint8_t val) { UDIEN = val; }
uint8_t NativeImpl::GetUDCON() const { return UDCON; }
void NativeImpl::SetUECFG0X(const uint8_t val) { UECFG0X = val; }
void NativeImpl::SetUEIENX(const uint8_t val) { UEIENX = val; }
uint8_t NativeImpl::GetUDADDR() const { return UDADDR; }
void NativeImpl::SetUDADDR(const uint8_t val) { UDADDR = val; }
void NativeImpl::SetUERST(const uint8_t val) { UERST = val; }

} // namespace native
} // namespace threeboard
