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

#ifndef _ROSA_LED_H_
#define _ROSA_LED_H_

#include <avr32/io.h>

#ifndef LED0_GPIO
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
 * LED API
 *
 * Comment:
 * 	Various functions for LED control
 *
 * valid 'lednr' parameters are:
 * LED0_GPIO	//Monocolor green, EVK1100 LED1
 * LED1_GPIO	//Monocolor green, EVK1100 LED2
 * LED2_GPIO	//Monocolor green, EVK1100 LED3
 * LED3_GPIO	//Monocolor green, EVK1100 LED4
 * LED4_GPIO	//Bicolor red, EVK1100 LED5
 * LED5_GPIO	//Bicolor green, EVK1100 LED5
 * LED6_GPIO	//Bicolor red, EVK1100 LED6
 * LED7_GPIO	//Bicolor green, EVK1100 LED6
 *
 **********************************************************/
//Initialize all LEDs, LEDx_GPIO
void ledInit(void);

//Turn a LED on
void ledOn(int lednr);

//Turn a LED off
void ledOff(int lednr);

//Toggle a LED
void ledToggle(int lednr);

#endif /* _ROSA_LED_H_ */
