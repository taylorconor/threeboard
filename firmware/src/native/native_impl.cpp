#include "native_impl.h"
#include <avr/interrupt.h>
#include <avr/io.h>
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

  // Enable global interrupts.
  sei();
}

void Timer3Init() {
  // Enable timer 3 in CTC mode, with prescaler set to 8.
  TCCR3B |= (1 << WGM32) | (1 << CS31);

  TCNT3 = 0;

  // Set the compare value. We want the interrupt to fire once every 5ms, since
  // the debounce code scans key states in 5ms increments. (5ms/0.0000625ms)/8 =
  // 10000.
  OCR3A = 9999;

  TIMSK3 |= (1 << OCIE3A);

  sei();
}
} // namespace

// Define the interrupt service registers (ISRs) for timers 1 and 3 (the
// atmega32u4 has no timer2, and timer0 is used for the system clock). The
// interrupt handlers are guaranteed to be defined because individual timer
// interrupts are enabled in the SetTimerNInterruptHandler methods which set the
// handler before enabling interrupts for that timer.
ISR(TIMER1_COMPA_vect) {
  NativeImpl::Get()->GetInterruptHandlerDelegate()->HandleTimer1Interrupt();
}
ISR(TIMER3_COMPA_vect) {
  NativeImpl::Get()->GetInterruptHandlerDelegate()->HandleTimer3Interrupt();
}

Native *NativeImpl::Get() {
  static NativeImpl instance = NativeImpl();
  return &instance;
}

void NativeImpl::SetTimer1InterruptHandler(InterruptHandlerDelegate *delegate) {
  delegate_ = delegate;
  Timer1Init();
}

void NativeImpl::SetTimer3InterruptHandler(InterruptHandlerDelegate *delegate) {
  delegate_ = delegate;
  Timer3Init();
}

InterruptHandlerDelegate *NativeImpl::GetInterruptHandlerDelegate() const {
  return delegate_;
}

void NativeImpl::Delay(uint8_t ms) const {
  while (ms-- > 0) {
    _delay_ms(1);
  }
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
} // namespace native
} // namespace threeboard
