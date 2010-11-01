/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// //;''//,
             ,///' '///,'/////',/////'  /////'\\,

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

#ifndef _ROSA_INT_H_
#define _ROSA_INT_H_

#include <avr32/io.h>

/***********************************************************
 * Kernel interrupt handling functions
 ***********************************************************/
//Interrupt initialization must be done before enabling interrupts
extern void interruptInit(void);

//Disable interrupts
extern void interruptDisable(void);

//Enable interrupts
extern void interruptEnable(void);

//Check if interrupts are enabled.
extern int  ROSA_isInterruptEnabled(void);

//Read the status register
//extern int _ROSA_readSR(void); TODO: Remove

void ROSA_yieldFromISR(void);

#endif /* _ROSA_INT_H_ */
