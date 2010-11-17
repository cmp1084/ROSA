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

#ifndef __ROSA_BUTTON_H_
#define __ROSA_BUTTON_H_

#include <avr32/io.h>
#include "rosa_config.h"

/***********************************************************
 * Button and joystick API
 *
 * Comment:
 * 	Various functions for reading the state of the buttons
 * 	and joystick
 *
 * Valid 'buttonnr' parameters are:
 * PUSH_BUTTON_0
 * PUSH_BUTTON_1
 * PUSH_BUTTON_2
 *
 **********************************************************/
void buttonInit(void);
void joystickInit(void);
int isButton(int buttonnr);
int isJoystickUp(void);
int isJoystickDown(void);
int isJoystickLeft(void);
int isJoystickRight(void);
int isJoystickPressed(void);

#endif /* __ROSA_BUTTON_H_ */
