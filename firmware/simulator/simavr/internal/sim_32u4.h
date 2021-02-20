/*
        sim_mega32u4.c

        Copyright 2008, 2009 Michel Pollet <buserror@gmail.com>

         This file is part of simavr.

        simavr is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
        (at your option) any later version.

        simavr is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with simavr.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "simavr/avr_acomp.h"
#include "simavr/avr_adc.h"
#include "simavr/avr_eeprom.h"
#include "simavr/avr_extint.h"
#include "simavr/avr_flash.h"
#include "simavr/avr_ioport.h"
#include "simavr/avr_spi.h"
#include "simavr/avr_timer.h"
#include "simavr/avr_twi.h"
#include "simavr/avr_uart.h"
#include "simavr/avr_usb.h"
#include "simavr/avr_watchdog.h"
#include "simavr/sim_avr.h"
#include "simulator/simavr/internal/iom32u4.h"
#include "simulator/simavr/internal/sim_core_declare.h"

#define SIM_VECTOR_SIZE 4
#define SIM_MMCU "atmega32u4"
#define SIM_CORENAME mcu_mega32u4

#define USBRF 5  // missing in avr/iom32u4.h

#define PD7 7
#define PD6 6
#define PD5 5
#define PD4 4
#define PD3 3
#define PD2 2
#define PD1 1
#define PD0 0
#define PE6 6

#define _AVR_IO_H_
#define __ASSEMBLER__
#ifndef __AVR_ATmega32U4__
#define __AVR_ATmega32U4__
#endif

typedef struct mcu_t {
  avr_t core;
  avr_eeprom_t eeprom;
  avr_flash_t selfprog;
  avr_watchdog_t watchdog;
  avr_extint_t extint;
  avr_ioport_t portb, portc, portd, porte, portf;
  avr_uart_t uart1;
  avr_acomp_t acomp;
  avr_adc_t adc;
  avr_timer_t timer0, timer1, timer3;
  avr_spi_t spi;
  avr_twi_t twi;
  avr_usb_t usb;
} mcu_t;
