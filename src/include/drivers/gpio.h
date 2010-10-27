/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///` ./// ///`///  ///`    ,, //
               ///////,  ///,///   '/// //;''//,
             ,///` '///,'/////`,/////`  /////`/;,

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

#ifndef __GPIO_H_
#define __GPIO_H_

//The maxiumim nr of pins on a GPIO port
#define GPIO_PIN_MAX 0x20

#define OUTPUT 0	//GPIO pin is output
#define INPUT 1		//GPIO pin is input
/***********************************************************
 * GPIO API
 *
 * Comment:
 * 	Various functions for GPIO control
 *
 **********************************************************/
//Initialize a specific GPIO pin
void gpioInit(int pinnr, int type);

//Set the value of a GPIO pin
void gpioSet(int pinnr);

//Clear the value of a GPIO pin
void gpioClear(int pinnr);

//Toggle the value of a GPIO pin
void gpioToggle(int pinnr);

//Get the value from a GPIO pin
int gpioGet(int pinnr);

#endif /* __GPIO_H_ */
