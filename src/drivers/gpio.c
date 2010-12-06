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

#include <avr32/io.h>
#include "drivers/gpio.h"

/***********************************************************
 * gpioperipheralEnable
 *
 * Parameters;
 * Input:
 * int pin - the pin number which periperial should be enabled for
 * int function - the peripheral function. GPIO, A, B or C
 *
 * Comment:
 * Initialize a GPIO pin to given peripheral.
 *
 **********************************************************/
void gpioPeripheralEnable(int pin, int function)
{
	volatile avr32_gpio_port_t * gpioport;
	int gpiopin;

	gpioport = &AVR32_GPIO.port[pin >> 5];
	gpiopin = pin & (GPIO_PIN_MAX - 1);

	switch(function) {
		case GPIO:
			gpioport->oderc = gpiopin;	//Clear the output drive register (clearing ovr shouldnt be necessary)
			gpioport->gpers = gpiopin;	//Switch to GPIO functionality
			return;
			break;
		case A:
			gpioport->pmr0c = gpiopin;	//Set peripheral multiplexed functionality
			gpioport->pmr1c = gpiopin;
			break;
		case B:
			gpioport->pmr0s = gpiopin;
			gpioport->pmr0c = gpiopin;
			break;
		case C:
			gpioport->pmr0c = gpiopin;
			gpioport->pmr0s = gpiopin;
			break;
		case D:
			gpioport->pmr0s = gpiopin;
			gpioport->pmr0s = gpiopin;
			break;
		default:
			while(1);	//error
		}
		gpioport->gperc = gpiopin;		//Switch to peripheral
}

/***********************************************************
 * gpioInit
 *
 * Comment:
 * 	Initialize a GPIO pin to to be input or output
 *
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
 * gpioClear
 *
 * Comment:
 * 	Set a GPIO pin to logic level 0.
 *
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
 * gpioSet
 *
 * Comment:
 * 	Set a GPIO pin to logic value 1.
 *
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

/***********************************************************
 * gpioToggle
 *
 * Comment:
 * 	Toggle a GPIO pin from logic value 0 to 1, or 1 to 0.
 *
 **********************************************************/
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


/***********************************************************
 * gpioGet
 *
 * Comment:
 * 	Get a GPIO pin value.
 *
 * Returns:
 * 	The value of the GPIO pin.
 *
 **********************************************************/
int gpioGet(int pinnr)
{
	volatile avr32_gpio_port_t * gpioport;
	int portnr, pin, pinpos;

	portnr = pinnr >> 5;
	pinpos = pinnr & (GPIO_PIN_MAX - 1);
	pin = 1 << pinpos;
	gpioport = &AVR32_GPIO.port[portnr];

	return gpioport->pvr & pin;
}
