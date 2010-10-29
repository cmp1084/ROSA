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

#ifndef _ROSA_TIMER_H_
#define _ROSA_TIMER_H_

#include <avr32/io.h>


/***********************************************************
 * Kernel timer lowlevel functions
 ***********************************************************/
extern void ROSA_timerInit(void);
extern void ROSA_timerReset(void);
extern void ROSA_timerStart(void);
extern void ROSA_timerStop(void);

//Read the TC0 timers status register
extern void ROSA_timerClearInterrupt(void);
extern void ROSA_timerPrescaleSet(int);
extern void ROSA_timerRCSet(int);
int ROSA_timerPeriodSet(unsigned int ms);

//The timer interrupt service routine
void ROSA_timerISR(void);

//Timer variables
extern int ROSA_timerPrescale;
extern int ROSA_timerRC;

#endif /* _ROSA_TIMER_H_ */
