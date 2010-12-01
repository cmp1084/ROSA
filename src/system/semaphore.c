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

#include <stdlib.h>
//~ #include "kernel/rosa_systick.h"
#include "include/system/semaphore.h"


/***********************************************************
 * ROSA_semCreate
 *
 * Comment:
 * Create a dynamic semaphore. Initiate it to be unlocked.
 *
 * Parameters:
 * In: sem ** semaphore - pointer to semaphore pointer, semaphore must be NULL when this function is called.
 *
 * Returns:
 * SEMOK if the semaphore was created successfully
 * SEMERROR if the semaphore was not created sucessfully
 *
 **********************************************************/
int ROSA_semCreate(sem ** semaphore)
{
	int interruptOnOff = isInterruptEnabled();
	interruptDisableIf(interruptOnOff);     //Make the semaphore operation atomic.

	if(*semaphore) {
		interruptEnableIf(interruptOnOff);  //Enable interrupts if they was _enabled_ previously. Otherwise, keep them disabled.
		return SEMERROR;
	}

	*semaphore = malloc(sizeof(sem));
	if(!*semaphore) {
		interruptEnableIf(interruptOnOff);  //Enable interrupts if they was _enabled_ previously. Otherwise, keep them disabled.
		return SEMERROR;
	}
	(*semaphore)->tcbHandle = NULL;        //No one uses the semaphore
	(*semaphore)->value = SEMUNLOCKED;     //The semaphore is not locked
	interruptEnableIf(interruptOnOff);     //Enable interrupts if they was _enabled_ previously. Otherwise, keep them disabled.
	return SEMOK;
}

/***********************************************************
 * ROSA_semCreateGlobal
 *
 * Comment:
 * Create a global semaphore. Initiate it to be unlocked.
 *
 * Parameters:
 * In: sem * semaphore - pointer to semaphore
 *
 * Returns:
 * SEMOK if the semaphore was created successfully
 * SEMERROR if the semaphore was not created sucessfully
 *
 **********************************************************/
int ROSA_semCreateGlobal(sem * semaphore)
{
	int interruptOnOff = isInterruptEnabled();
	interruptDisableIf(interruptOnOff);     //Make the semaphore operation atomic.

	if(!semaphore) {
		interruptEnableIf(interruptOnOff);  //Enable interrupts if they was _enabled_ previously. Otherwise, keep them disabled.
		return SEMERROR;
	}
	(semaphore)->tcbHandle = NULL;        //No one uses the semaphore
	(semaphore)->value = SEMUNLOCKED;     //The semaphore is not locked
	interruptEnableIf(interruptOnOff);    //Enable interrupts if they was _enabled_ previously. Otherwise, keep them disabled.
	return SEMOK;
}
/***********************************************************
 * ROSA_semDestroy
 *
 * Comment:
 * sem * semaphore - A pointer to the semaphore to destroy
 *
 **********************************************************/
int ROSA_semDestroy(sem ** semaphore)
{
	int interruptOnOff = isInterruptEnabled();

	interruptDisableIf(interruptOnOff);
	if(*semaphore == NULL) {
		interruptEnableIf(interruptOnOff);
		return SEMERROR;
	}
	if((*semaphore)->value != SEMUNLOCKED) {
		interruptEnableIf(interruptOnOff);
		return SEMLOCKED;
	}
	if((*semaphore)->tcbHandle == EXECTASK) {
		free(*semaphore);
		*semaphore = NULL;
		interruptEnableIf(interruptOnOff);
		return SEMOK;
	}

	interruptEnableIf(interruptOnOff);
	return SEMERROR;
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
int ROSA_semGive(sem * semaphore)
{
	int interruptOnOff = isInterruptEnabled();

	if(!semaphore) {
		return SEMERROR;    //SEMERROR the semaphore does not exist
	}

	interruptDisableIf(interruptOnOff);
	if(semaphore->tcbHandle != EXECTASK) {    //Only the owner may return a s/emaphore. Oh! How bad!!! >:/
		interruptEnableIf(interruptOnOff);
		return SEMERROR;
	}

	semaphore->value = SEMUNLOCKED;
	if(semaphore->value == SEMUNLOCKED) {
		semaphore->tcbHandle = NULL;
	}

	interruptEnableIf(interruptOnOff);
	return SEMOK;
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
 * SEMERROR on failure to take the semaphore
 * SEMOK when semaphore have been taken.
 **********************************************************/
int ROSA_semTake(sem * semaphore)
{
	int interruptOnOff;

	if(!semaphore) {
		return SEMERROR;    //SEMERROR, The semaphore does not exist
	}

	interruptOnOff = isInterruptEnabled();
	if(semaphore->tcbHandle != NULL) {
		interruptEnableIf(interruptOnOff);
		return SEMERROR;
	}
	semaphore->tcbHandle = EXECTASK;
	semaphore->value = SEMLOCKED;
	interruptEnableIf(interruptOnOff);
	return SEMOK;
}


//~ void ROSA_semGiveTo(Tcb * tcbblock, sem * semaphore) TODO: Remove
//~ {
	//~ //If current task own the semaphore, give it to another task
	//~ if(semaphore->tcbHandle == EXECTASK) {
		//~ semaphore->tcbHandle = tcbblock;
	//~ }
//~ }