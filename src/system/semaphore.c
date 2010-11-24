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

/*
File creation date: 20101108 15:41:45

*/

#include "kernel/rosa_systick.h"
#include "system/semaphore.h"

//Debugging //TODO: Remove
#include "drivers/led.h"

/***********************************************************
 * ROSA_semCreate
 *
 * Comment:
 * Create a semaphore. Initiate it to be unlocked.
 *
 * Backdraw:
 * void function, can not signal errors in a good way.
 *
 **********************************************************/
void ROSA_semCreate(sem * semaphore)
{
	int interruptOnOff = isInterruptEnabled();
	interruptDisableIf(interruptOnOff); //Make the semaphore operation atomic.
	while(semaphore == NULL);            //Jam here if a NULL semaphore is detected
	semaphore->tcbHandle = NULL;        //No one uses the semaphore
	semaphore->value = UNLOCKED;        //The semaphore is not locked
	interruptEnableIf(interruptOnOff);  //Enable interrupts if they was _enabled_ previously too. Otherwise, keep them disabled.
}

/***********************************************************
 * ROSA_semDestroy
 *
 * Comment:
 * sem * semaphore - A pointer to the semaphore to destroy
 *
 **********************************************************/
int ROSA_semDestroy(sem * semaphore)
{
	int interruptOnOff = isInterruptEnabled();
	if(semaphore == NULL) {
		return ERROR;
	}
	if(semaphore->value != UNLOCKED) {
		interruptEnableIf(interruptOnOff);
		return ERROR;
	}
	if(semaphore->tcbHandle == EXECTASK) {
		interruptEnableIf(interruptOnOff);
		return OK;
	}
	interruptEnableIf(interruptOnOff);
	return ERROR;
}

/***********************************************************
 * ROSA_semGive
 *
 * Comment:
 * Give a semaphore back.
 *
 * Parameters:
 * sem * semaphore - A pointer to the semaphore to give back.
 *
 **********************************************************/
void ROSA_semGive(sem * semaphore)
{

	int interruptOnOff = isInterruptEnabled();
	if(semaphore->tcbHandle != EXECTASK) {    //Only the owner may return a semaphore. Oh! How bad!!! :(
		//ledOn(LED7_GPIO);
		//while(1);	//Error, we didnt own the semaphore
		interruptEnableIf(interruptOnOff);
		return;
	}
	semaphore->value = UNLOCKED;
	if(semaphore->value == UNLOCKED) {
		semaphore->tcbHandle = NULL;
	}
	interruptEnableIf(interruptOnOff);
	return;
}

/***********************************************************
 * ROSA_semTake
 *
 * Comment:
 * Take a binary semaphore if it have been created
 * and no one else have taken it.
 *
 * Parameters:
 * sem * semaphore - A pointer to the semaphore to take.
 *
 * Return values:
 * ERROR on failure to take the semaphore
 * OK when semaphore have been taken.
 **********************************************************/
int ROSA_semTake(sem * semaphore)
{
	int interruptOnOff;

	if(!semaphore) {
		return ERROR;
		//~ while(1);    //Error, The semaphore does not exist
	}

	interruptOnOff = isInterruptEnabled();
	if(semaphore->tcbHandle != NULL) {
		interruptEnableIf(interruptOnOff);
		return ERROR;
	}
	semaphore->tcbHandle = EXECTASK;
	semaphore->value = LOCKED;
	interruptEnableIf(interruptOnOff);
	return OK;
}

//~ void ROSA_semGiveTo(Tcb * tcbblock, sem * semaphore) TODO: Remove
//~ {
	//~ //If current task own the semaphore, give it to another task
	//~ if(semaphore->tcbHandle == EXECTASK) {
		//~ semaphore->tcbHandle = tcbblock;
	//~ }
//~ }