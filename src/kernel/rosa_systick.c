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

#include "kernel/rosa_systick.h"

//Timer tick interrupt routine
static int sysTick = 0;
/***********************************************************
 *
 **********************************************************/
void _sysTickIncrease(void)
{
	sysTick++;
}

/***********************************************************

 **********************************************************/
unsigned int ROSA_sysTickGet(void)
{
	return sysTick;
}

/***********************************************************

 **********************************************************/
void _sysTickReset(void)
{
	sysTick = 0;
}

/***********************************************************

 **********************************************************/
void ROSA_wait(unsigned int ticks)
{
	unsigned int now;
	now = ROSA_sysTickGet();
	while(ROSA_sysTickGet() < (now + ticks)) {
		ROSA_yield();
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
void ROSA_waitUntil(unsigned int absoluteTick)
{
	while(ROSA_sysTickGet() < absoluteTick) {
		ROSA_yield();
	}
}
