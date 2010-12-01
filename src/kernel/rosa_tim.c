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

#include "rosa_config.h"
#include "drivers/delay.h"
#include "kernel/rosa_int.h"
#include "kernel/rosa_tim.h"

/***********************************************************
 * timerInterruptHandler
 *
 * Comment:
 * 	This is the basic timer interrupt service routine.
 **********************************************************/
//TODO: Having a lot of local variables here WILL mess up the ROSA_yieldFromISR, contextSaveFromISR and contextRestoreFromISR
//Global variables are ofcourse ok.
//
//It would be better to have the _int0 routine call an assembler routine that call a C function, to get rid of these ridiculous problems.
__attribute__((__interrupt__))
void handler_timerISR(void)
{
	int sr;
	volatile avr32_tc_t * tc = &AVR32_TC;

	//Read the timer status register to determine if this is a valid interrupt
	sr = tc->channel[0].sr;

	if(sr & AVR32_TC_CPCS_MASK) {
		_sysTickIncrease();
		ROSA_yieldFromISR();
	}
}


/***********************************************************
 * timerPeriodSet
 *
 * Comment:
 * 	Set the timer period to 'ms' milliseconds.
 *
 **********************************************************/
int timerPeriodSet(unsigned int ms)
{
	int rc, prescale;
	int f[] = { 2, 8, 32, 128 };                    //Prescale factor
	//FOSC0 / factor_prescale * time[s];
	prescale = AVR32_TC_CMR0_TCCLKS_TIMER_CLOCK5;   //Set the prescale factor
	rc = FOSC0 / f[prescale - 1] * ms / 1000;       //Calculate the RC (register C compare value)
	timerPrescaleSet(prescale);
	timerRCSet(rc);
	return rc * prescale / FOSC0;                   //Return the calculated period [s]
}

unsigned int timerPeriodGet(void)
{
	return timerRC * timerPrescale / FOSC0;
}