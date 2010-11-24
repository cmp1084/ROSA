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

#include "drivers/led.h"

/**********************************************************
 * ledInit
 *
 * Comment:
 * 	Initialize all EVK1100 LEDs.
 *
 * Returns:
 * 	Nothing
 *
 *********************************************************/
void ledInit(void)
{
	int leds[] = {
		LED0_GPIO,
		LED1_GPIO,
		LED2_GPIO,
		LED3_GPIO,
		LED4_GPIO,
		LED5_GPIO,
		LED6_GPIO,
		LED7_GPIO
	};

	int nr = 0;
	do {
		ledOff(leds[nr]);
		gpioInit(leds[nr], OUTPUT);
	} while(nr++ != LEDMAX - 1);
}


/**********************************************************
 * ledOn
 *
 * Comment:
 * 	Make a LED light up
 *
 * Returns:
 * 	Nothing
 *
 *********************************************************/
void ledOn(int lednr) { gpioClear(lednr); }


/**********************************************************
 * ledOff
 *
 * Comment:
 * 	Make a LED light down
 *
 * Returns:
 * 	Nothing
 *
 *********************************************************/
void ledOff(int lednr) { gpioSet(lednr); }


/**********************************************************
 * ledToggle
 *
 * Comment:
 * 	Make a LED toggle its light on or off, depending on
 * 	previous state.
 *
 * Returns:
 * 	Nothing
 *
 *********************************************************/
void ledToggle(int lednr) { gpioToggle(lednr); }
