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

#ifndef _USART_H_
#define _USART_H_

#include <avr32/io.h>
#include "kernel/rosa_int.h"
#include "rosa_config.h"

#define USART_NO_RX_CHAR 0

//Return Values
#define USART_SUCCESS			0 //Successful completion.
#define USART_FAILURE			1 //Failure because of some unspecified reason.
#define USART_INVALID_INPUT		1 //Input value out of range.
#define USART_INVALID_ARGUMENT	1 //Argument value out of range.
#define USART_TX_BUSY			2 //Transmitter was busy.
#define USART_RX_EMPTY			3 //Nothing was received.
#define USART_RX_ERROR			4 //Transmission error occurred.
#define USART_MODE_FAULT		5 //USART not in the appropriate mode.

//Parity Settings
#define USART_EVEN_PARITY		AVR32_USART_MR_PAR_EVEN		//Use even parity on character transmission.
#define USART_ODD_PARITY		AVR32_USART_MR_PAR_ODD		//Use odd parity on character transmission.
#define USART_SPACE_PARITY		AVR32_USART_MR_PAR_SPACE	//Use a space as parity bit.
#define USART_MARK_PARITY		AVR32_USART_MR_PAR_MARK		//Use a mark as parity bit.
#define USART_NO_PARITY			AVR32_USART_MR_PAR_NONE		//Don't use a parity bit.
#define USART_MULTIDROP_PARITY	AVR32_USART_MR_PAR_MULTI	//Parity bit is used to flag address characters.

//Stop Bits Settings
#define USART_1_STOPBIT			AVR32_USART_MR_NBSTOP_1		//Use 1 stop bit.
#define USART_2_STOPBITS		AVR32_USART_MR_NBSTOP_2		//Use 2 stop bits

//Channel Modes
#define USART_NORMAL_CHMODE		AVR32_USART_MR_CHMODE_NORMAL	//Normal communication.

//Input parameters when initializing USART
typedef struct
{
	unsigned long baudrate;
	unsigned char charlength;
	unsigned char paritytype;
	unsigned short stopbits;
	unsigned char channelmode;
} usart_options_t;

//USART options.
static const usart_options_t usart_options = {
	.baudrate    = USART_BAUDRATE,
	.charlength  = USART_CHARLEN,
	.paritytype  = USART_NO_PARITY,
	.stopbits    = USART_1_STOPBIT,
	.channelmode = USART_NORMAL_CHMODE,
};

void usartReset(volatile avr32_usart_t *usart);
int usartSetBaudrate(volatile avr32_usart_t *usart, unsigned int baudrate, unsigned long pba_hz);
int usartInit(volatile avr32_usart_t *usart, const usart_options_t *opt, long pba_hz);
void usartGetLine(volatile avr32_usart_t * usart, char * buf);
char usartGetChar(volatile avr32_usart_t * usart);
void usartWriteLine(volatile avr32_usart_t * usart, char * string);
void usartWriteChar(volatile avr32_usart_t * usart, char ch);
void usartWriteTcb(volatile avr32_usart_t * usart, tcb * dbgtcb);

#endif /* _USART_H_ */
