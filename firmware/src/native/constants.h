#pragma once

#include <stdint.h>

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

// Port C
constexpr uint8_t PC6 = 6;
constexpr uint8_t PC7 = 7;

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
constexpr uint8_t STALLEDI = 1;
constexpr uint8_t RXOUTI = 2;
constexpr uint8_t RXSTPI = 3;
constexpr uint8_t NAKOUTI = 4;
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

// UCSR1A
constexpr uint8_t UDRE1 = 5;
constexpr uint8_t TXC1 = 6;

// UCSR1B
constexpr uint8_t TXEN1 = 3;

// UCSR1C
constexpr uint8_t UCSZ10 = 1;
constexpr uint8_t UCSZ11 = 2;

// TWCR
constexpr uint8_t TWEN = 2;
constexpr uint8_t TWSTO = 4;
constexpr uint8_t TWSTA = 5;
constexpr uint8_t TWEA = 6;
constexpr uint8_t TWINT = 7;

// TWSR
constexpr uint8_t TW_START = 0x08;
constexpr uint8_t TW_REP_START = 0x10;
constexpr uint8_t TW_MT_SLA_ACK = 0x18;
constexpr uint8_t TW_MT_SLA_NACK = 0x20;
constexpr uint8_t TW_MT_DATA_ACK = 0x28;
constexpr uint8_t TW_MT_DATA_NACK = 0x30;
constexpr uint8_t TW_MT_ARB_LOST = 0x38;
constexpr uint8_t TW_MR_ARB_LOST = 0x38;
constexpr uint8_t TW_MR_SLA_ACK = 0x40;
constexpr uint8_t TW_MR_SLA_NACK = 0x48;
constexpr uint8_t TW_MR_DATA_ACK = 0x50;
constexpr uint8_t TW_MR_DATA_NACK = 0x58;
constexpr uint8_t TW_ST_SLA_ACK = 0xA8;
constexpr uint8_t TW_ST_ARB_LOST_SLA_ACK = 0xB0;
constexpr uint8_t TW_ST_DATA_ACK = 0xB8;
constexpr uint8_t TW_ST_DATA_NACK = 0xC0;
constexpr uint8_t TW_ST_LAST_DATA = 0xC8;
constexpr uint8_t TW_SR_SLA_ACK = 0x60;
constexpr uint8_t TW_SR_ARB_LOST_SLA_ACK = 0x68;
constexpr uint8_t TW_SR_GCALL_ACK = 0x70;
constexpr uint8_t TW_SR_ARB_LOST_GCALL_ACK = 0x78;
constexpr uint8_t TW_SR_DATA_ACK = 0x80;
constexpr uint8_t TW_SR_DATA_NACK = 0x88;
constexpr uint8_t TW_SR_GCALL_DATA_ACK = 0x90;
constexpr uint8_t TW_SR_GCALL_DATA_NACK = 0x98;
constexpr uint8_t TW_SR_STOP = 0xA0;
constexpr uint8_t TW_NO_INFO = 0xF8;

}  // namespace native
}  // namespace threeboard
