/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// ///''\\,
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

#include "app/warning.h"

static int warningLedNr;
static int warningHoldTime
static sem_warning;

/***********************************************************
 * warning
 *
 * Comment:
 * Create a semaphore. Initiate it to be unlocked.
 *
 *
 **********************************************************/
void warning(int lednr, int holdTime)
{
	warningLedNr = lednr;
	warningHoldTime = holdTime;
	ROSA_taskAdd(NULL, "warn", taskWarning, NULL, 20);
}

void warningWait(unsigned int ticks, led)
{
	unsigned int now;
	now = ROSA_sysTickGet();
	while(ROSA_sysTickGet() <= (now + ticks)) {
		ledOn(
		ROSA_yield();
	}
}


/***********************************************************
 * taskWarning
 *
 * Comment:
 * This is the task that display the warning.
 *
 * TODO:
 * No other warnings are allowed to run while this warning
 * is running. This might case delays in later warnings.
 *
 **********************************************************/
void taskWarning(void)
{
	unsigned int now;

	while(!ROSA_semTake(&sem_warning));
	now = ROSA_sysTickGet();
	while(ROSA_sysTickGet() <= (now + warningHoldTime)) {
		ledOn(warningLedNr);
		ROSA_yield();
	}
	ROSA_semGive(&sem_warning);
	ROSA_taskDestroy();
}