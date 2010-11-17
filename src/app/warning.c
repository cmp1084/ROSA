/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// ///''//,
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

/***********************************************************
 * warning system
 *
 * Comment:
 * This warning system make each LED able to signal a warning.
 * Consecutive warnings on the same LED will have to wait
 * until the previous warnings hold time have passed.
 *
 **********************************************************/

//Include system files
#include "system/semaphore.h"
#include "kernel/rosa_dyn.h"
#include "kernel/rosa_systick.h"
#include "kernel/rosa_tim.h"

//Include driver files
#include "drivers/led.h"

//Include application files
#include "app/warning.h"

#include "rosa_config.h"

//Private variables
#define WARNINGMAX LEDMAX
int leds[WARNINGMAX] = { LED0_GPIO, LED1_GPIO, LED2_GPIO, LED3_GPIO, LED4_GPIO, LED5_GPIO, LED6_GPIO, LED7_GPIO };
int warningHoldTime[WARNINGMAX];
sem sem_warning[WARNINGMAX];
sem sem_warningparams;
int warningNr;			//Parameter value to the taskWarning is passed through this variable

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
	int nr;
	unsigned int realTickTime, ticks, now;

	//sem_warning[nr] locked at this point
	//sem_warningparams locked at this point
	//Get the parameters for this task
	nr = warningNr;

	//Done with the params, give the semaphore back
	//ROSA_semGive(&sem_warningparams);			//BAD, we cant do this, since we dont own the task. Ok, we fix something up.

	//Translate warningHoldTime [ms] to ticks
	realTickTime = timerPeriodGet();
	ticks = warningHoldTime[nr] / realTickTime;

	//~ usartWriteLine(USART, "Warning"
	//Wait warningHoldTime ticks
	now = ROSA_sysTickGet();
	while(ROSA_sysTickGet() < (now + ticks)) {
		ledOn(leds[nr]);
		ledOn(LED6_GPIO);
		ROSA_yield();
	}

	//Close down warning
	ledOff(leds[nr]);
	ledOff(LED6_GPIO);
	//ROSA_semGive(&sem_warning[nr]);
	ROSA_taskDestroy();
}

/***********************************************************
 * warning
 *
 * Comment:
 * Create a semaphore. Initiate it to be unlocked.
 *
 * In:
 * int lednr - Which LED to light up by this warning.
 *
 **********************************************************/
void warning(int nr, int holdTime)
{
	Tcb * tcb_taskwarning;
	//Wait for any previous warnings to complete
	//while(!ROSA_semTake(&sem_warning[nr]));
	//Add new warning, wait until it is safe to pass the params
	//while(!ROSA_semTake(&sem_warningparams));
	warningNr = nr;							//Param to send to taskWarning
	warningHoldTime[nr] = holdTime;
	tcb_taskwarning = ROSA_taskAdd(NULL, "warn", taskWarning, NULL, 40);
	ROSA_semGiveTo(tcb_taskwarning, &sem_warningparams);
	ROSA_semGiveTo(tcb_taskwarning, &sem_warning[nr]);
}

void warningInit(void)
{
	int i;
	for(i = 0; i < LEDMAX; ++i) {
		ROSA_semCreate(&sem_warning[i]);
	}
	ROSA_semCreate(&sem_warningparams);
}