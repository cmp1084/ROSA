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

#define YES 1
#define NO 0

/***********************************************************
 * TCB id configures
 **********************************************************/
//Maximum number of chars in the task id (the task name)
#define CONFIG_NAMESIZE 4

/***********************************************************
 * Debug output configures
 **********************************************************/
//Which debug level will output from the various dgbXYZ() functions be?
//Valid values are DEBUGLEVEL0-DEBUGLEVEL3
#define DEBUGLEVEL DEBUGLEVEL1

/***********************************************************
 * Push Button GPIO configures
 **********************************************************/
#define CONFIG_BUTTON YES

#if(CONFIG_BUTTON)
#define PUSH_BUTTON_0	AVR32_PIN_PX16
#define PUSH_BUTTON_1	AVR32_PIN_PX19
#define PUSH_BUTTON_2	AVR32_PIN_PX22
#endif

/***********************************************************
 * Joystick GPIO configures
 **********************************************************/
#define CONFIG_JOYSTICK YES

#if(CONFIG_JOYSTICK)
#define JOYSTICK_PRESS	AVR32_PIN_PA20
#define JOYSTICK_LEFT	AVR32_PIN_PA25
#define JOYSTICK_RIGHT	AVR32_PIN_PA28
#define JOYSTICK_UP		AVR32_PIN_PA26
#define JOYSTICK_DOWN	AVR32_PIN_PA27
#endif

/***********************************************************
 * LED GPIO configures
 **********************************************************/
#define CONFIG_LED YES

#if(CONFIG_LED)
//The number of LEDs to control
#define LEDMAX 8					//The number of LEDs on the EVK1100
#define LED0_GPIO	AVR32_PIN_PB27	//LED1 on the EVK1100
#define LED1_GPIO	AVR32_PIN_PB28	//LED2
#define LED2_GPIO	AVR32_PIN_PB29	//...and so on
#define LED3_GPIO	AVR32_PIN_PB30
#define LED4_GPIO	AVR32_PIN_PB19
#define LED5_GPIO	AVR32_PIN_PB20
#define LED6_GPIO	AVR32_PIN_PB21
#define LED7_GPIO	AVR32_PIN_PB22
#endif

/***********************************************************
 * POT configures
 **********************************************************/
#define CONFIG_POT YES

/***********************************************************
 * Timer configures
 **********************************************************/
#define FOSC0 12000000		//CPU clock frequencey

#define CONFIG_TIMER YES

#if(CONFIG_TIMER)
#define PERIODTIME_MS 10
#endif

/***********************************************************
 * USART configures
 **********************************************************/
#define CONFIG_USART0 YES
#define CONFIG_USART1 YES

#if(CONFIG_USART0)
#define USART0              (&AVR32_USART0)
#define USART0_BAUDRATE     115200	//57600
#define USART0_CHARLEN      8
#define USART0_RX_PIN       AVR32_USART0_RXD_0_0_PIN
#define USART0_RX_FUNCTION  AVR32_USART0_RXD_0_0_FUNCTION
#define USART0_TX_PIN       AVR32_USART0_TXD_0_0_PIN
#define USART0_TX_FUNCTION  AVR32_USART0_TXD_0_0_FUNCTION
#endif

#if(CONFIG_USART1)
#define USART1              (&AVR32_USART1)
#define USART1_BAUDRATE     115200	//57600
#define USART1_CHARLEN      8
#define USART1_RX_PIN       AVR32_USART1_RXD_0_0_PIN
#define USART1_RX_FUNCTION  AVR32_USART1_RXD_0_0_FUNCTION
#define USART1_TX_PIN       AVR32_USART1_TXD_0_0_PIN
#define USART1_TX_FUNCTION  AVR32_USART1_TXD_0_0_FUNCTION
#endif

/***********************************************************
 * Task default priority configures
 **********************************************************/
#define IDLEPRIORITY 0
#define DEFAULTLOWPRIO 1
#define TASKINITIALPRIORITY 1

#define DEFAULTRELEASETIME 0

#endif /* _ROSA_CONFIG_H_ */
