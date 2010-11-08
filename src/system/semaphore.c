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

#include "system/semaphore.h"

#define OK 0
#define ERROR -1

#define LOCKED 1
#define UNLOCKED 0


/***********************************************************
 * semInterruptDisable
 *
 * Comment:
 * Disable interrupts and return TRUE or FALSE
 * depending on if interrupt was previously enabled or not.
 *
 **********************************************************/
int semInterruptDisable(void)
{
	int interrupt = isInterruptEnabled();
	interruptDisable();
	return interrupt;
}

/***********************************************************
 * semInterruptEnable
 *
 * Comment:
 * Enable interrupts, if 'interrupt' is TRUE.
 *
 **********************************************************/
void semInterruptEnable(int interrupt)
{
	if(interrupt) {
		interruptEnable();
	}
}

/***********************************************************
 * ROSA_semCreate
 *
 * Comment:
 * Create a semaphore. Initiate it to be unlocked.
 *
 **********************************************************/
void ROSA_semCreate(sem * semaphore)
{
	int interrupt =	semInterruptDisable();
	semaphore->tcbHandle = NULL;			//No one uses the semaphore
	semaphore->value = UNLOCKED;	//The semaphore is not locked
	semInterruptEnable(interrupt);	//Enable interrupts if they was enabled previously
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
	int interrupt =	semInterruptDisable();
	if(semaphore->value != UNLOCKED) {
		semInterruptEnable(interrupt);
		return ERROR;
	}
	if(semaphore->tcbHandle == EXECTASK) {
		semInterruptEnable(interrupt);
		return OK;
	}
	semInterruptEnable(interrupt);
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
	int interrupt =	semInterruptDisable();
	if(semaphore->tcbHandle != EXECTASK) {
		semInterruptEnable(interrupt);
		return;
	}
	semaphore->value = UNLOCKED;		//-- to make a counting semaphore
	if(semaphore->value == UNLOCKED) {
		semaphore->tcbHandle = NULL;
	}
	semInterruptEnable(interrupt);
	return;
}

/***********************************************************
 *
 *
 * Comment:
 *
 **********************************************************/
int ROSA_semTake(sem * semaphore)
{
	int interrupt =	semInterruptDisable();
	if(semaphore->tcbHandle != NULL) {
		semInterruptEnable(interrupt);
		return ERROR;
	}
	semaphore->tcbHandle = EXECTASK;
	semaphore->value = LOCKED;		// ++ to make a counting semaphore
	semInterruptEnable(interrupt);
	return OK;
}
