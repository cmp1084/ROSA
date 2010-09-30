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

#include <avr32/io.h>
#include "drivers/gpio.h"

/***********************************************************
 * ROSA_timerPrescaleSet
 *
 * Comment:
 * 	Set the prescale value of the timer
 *
 * C prototypes:
 *  extern void ROSA_timerPrescaleSet(void);
 **********************************************************/
void gpioInit(int pinnr, int type)
{
	volatile avr32_gpio_port_t * gpioport;
	int portnr, pin, pinpos;

	portnr = pinnr >> 5;
	pinpos = pinnr & (GPIO_PIN_MAX - 1);
	pin = 1 << pinpos;
	gpioport = &AVR32_GPIO.port[portnr];

	gpioport->gpers = pin;
	gpioport->pmr0c = pin;
	gpioport->pmr1c = pin;

	switch(type) {
		case OUTPUT:
			gpioport->oders = pin;
			break;
		case INPUT:
			gpioport->oderc = pin;
			break;
		default:
			gpioport->oderc = pin;
			break;
	}
}
/***********************************************************
 * ROSA_timerPrescaleSet
 *
 * Comment:
 * 	Set the prescale value of the timer
 *
 * C prototypes:
 *  extern void ROSA_timerPrescaleSet(void);
 **********************************************************/
void gpioClear(int pinnr)
{
	volatile avr32_gpio_port_t * gpioport;
	int portnr, pin, pinpos;

	portnr = pinnr >> 5;
	pinpos = pinnr & (GPIO_PIN_MAX - 1);
	pin = 1 << pinpos;
	gpioport = &AVR32_GPIO.port[portnr];

	gpioport->ovrc = pin;
}
/***********************************************************
 * ROSA_timerPrescaleSet
 *
 * Comment:
 * 	Set the prescale value of the timer
 *
 * C prototypes:
 *  extern void ROSA_timerPrescaleSet(void);
 **********************************************************/
void gpioSet(int pinnr)
{
	volatile avr32_gpio_port_t * gpioport;
	int portnr, pin, pinpos;

	portnr = pinnr >> 5;
	pinpos = pinnr & (GPIO_PIN_MAX - 1);
	pin = 1 << pinpos;
	gpioport = &AVR32_GPIO.port[portnr];

	gpioport->ovrs = pin;
}

void gpioToggle(int pinnr)
{
	volatile avr32_gpio_port_t * gpioport;
	int portnr, pin, pinpos;

	portnr = pinnr >> 5;
	pinpos = pinnr & (GPIO_PIN_MAX - 1);
	pin = 1 << pinpos;
	gpioport = &AVR32_GPIO.port[portnr];

	gpioport->ovrt = pin;
}

int gpioPVR(int pinnr)
{
	volatile avr32_gpio_port_t * gpioport;
	int portnr, pin, pinpos;

	portnr = pinnr >> 5;
	pinpos = pinnr & (GPIO_PIN_MAX - 1);
	pin = 1 << pinpos;
	gpioport = &AVR32_GPIO.port[portnr];

	return gpioport->pvr & pin;
}
