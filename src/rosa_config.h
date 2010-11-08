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
#define USART_BAUDRATE 		57600
#define USART_CHARLEN		8
#define USART_RX_PIN		AVR32_USART0_RXD_0_0_PIN
#define USART_RX_FUNCTION	AVR32_USART0_RXD_0_0_FUNCTION
#define USART_TX_PIN		AVR32_USART0_TXD_0_0_PIN
#define USART_TX_FUNCTION	AVR32_USART0_TXD_0_0_FUNCTION

#define FOSC0 12000000		//CPU clock frequencey

#endif /* _ROSA_CONFIG_H_ */
