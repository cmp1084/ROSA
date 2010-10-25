/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// //;''//,
             ,///' '///,'/////',/////'  /////'/;,

    Copyright 2010 Marcus Jansson <mjansson256@yahoo.se>

    This file is part of ROSA - Realtime Operating System for AVR32.

    ROSA is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ROSA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ROSA.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/
/* Tab size: 4 */

#ifndef _ROSA_CONFIG_H_
#define _ROSA_CONFIG_H_

#include <avr32/io.h>
#include "drivers/debug.h"

//Maximum number of chars in the task id (the task name)
#define NAMESIZE 4

//Which debug level will output from the various dgbXYZ() functions be?
//Valid values are DEBUGLEVEL0-DEBUGLEVEL3
#define DEBUGLEVEL DEBUGLEVEL1

//USART configures
#define USART				(&AVR32_USART0)
#define USART_RX_PIN		AVR32_USART0_RXD_0_0_PIN
#define USART_RX_FUNCTION	AVR32_USART0_RXD_0_0_FUNCTION
#define USART_TX_PIN		AVR32_USART0_TXD_0_0_PIN
#define USART_TX_FUNCTION	AVR32_USART0_TXD_0_0_FUNCTION
#define USART_BAUDRATE 57600
#define USART_CHARLEN 8

/***********************************************************
 * Timer configure
 * --------------------
 * FOSC0
 * TIMERPRESCALE_CONFIG
 * TIMERRC
 *
 * Comment:
 *	FOSC
 * 	--------
 * 	CPU clock frequency, in Hz.
 *	This is a fix value of 12000000 Hz.
 *
 * 	Prescale
 * 	---------
 *	Prescale values can be:
 * 	TC_CLOCK_SOURCE_TC2,	//fPBA / 2 //TODO: _CLOCKSx
 * 	TC_CLOCK_SOURCE_TC3,	//fPBA / 8
 * 	TC_CLOCK_SOURCE_TC4,	//fPBA / 32
 * 	TC_CLOCK_SOURCE_TC5,	//fPBA / 128
 *	This correspond to a PBA clock (= CPU clock) divisor of
 * 	{2, 8, 32, 128} respectively.
 *
 *	RC
 *	---------
 *	This is the compare value that give timer interrupts.
 * 	Valid values are in the interval 1-65535.
 *
 *	How to calculate timer period:
 *	-------------------------------
 *	FOSC0 / timerPrescale * time[s];
 *	To get 0.5 s: 12000000 / 128 * 0.5 = 46875 = timerRC
 *
 **********************************************************/
#define FOSC0 12000000											//CPU clock frequencey
#define TIMERPRESCALE_CONFIG AVR32_TC_CMR0_TCCLKS_TIMER_CLOCK5 	//equals TC_CLOCK_SOURCE_TC5
#define TIMERRC_CONFIG	46875 / 2								//Corresponding to 0.5 s period at prescale _CLOCKS5

#endif /* _ROSA_CONFIG_H_ */
