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

#include <stdlib.h>				//malloc, free
#include "kernel/rosa_dyn.h"
#include "kernel/rosa_int.h"
#include "kernel/rosa_scheduler.h"


//Only used during debug
//Driver includes
//~ #include "drivers/led.h" TODO: REMOVE


#include "rosa_config.h"

extern void contextRestore(void);           //rosa_ker_asm.S
extern void taskDestroyed(void);            //rosa_ker_asm.S
extern void prioSet(Tcb * tcb, int prio);   //rosa_ker.c

/***********************************************************
 * Comment:
 * Internal functions for simple bookkeeping of the dynamic
 * tasks. This just keep track of the number of dynamic
 * tasks in the system.
 *
 * Adds int ROSA_dynTaskNrGet(void) to the API.
 *
 **********************************************************/
//Counter for dynamic tasks
static int _dynTaskNr = 0;

int ROSA_dynTaskNrGet(void) { return _dynTaskNr; }
void _dynTaskNrInc(void) { _dynTaskNr++; }
void _dynTaskNrDec(void) { _dynTaskNr--; }


/***********************************************************
 * ROSA_taskCreate
 *
 * Comment:
 * Create a dynamic task, without passing Tcb or stack to
 * the function.
 *
 * Ref. ROSA_taskAdd(Tcb * tcb, ...)
 *
 **********************************************************/
Tcb * ROSA_taskCreate(const char * id, const void * taskFunction, const int prio, const int stackSize)
{
	Tcb * tcb;
	void * stack;
	int interruptOnOff;

	interruptOnOff = isInterruptEnabled();
	interruptDisableIf(interruptOnOff);

	//Allocate some memory for tcb and stack
	tcb = malloc(sizeof(Tcb));
	stack = malloc(stackSize * sizeof(int));
	if(!tcb && !stack) {
		free(tcb);
		free(stack);
		return NULL;	//return ERROR; malloc memory error.
	}

	//Create and install the dynamic tcb
	ROSA_tcbCreate(tcb, id, taskFunction, stack, stackSize);
	//ROSA_tcbInstall(tcb);

	//Set the task priority
	prioSet(tcb, prio);

	//Insert in the ready state
	//~ heapInsert(readyHeap, tcb);
	addToScheduler(tcb);

	_dynTaskNrInc();
	interruptEnableIf(interruptOnOff);
	return tcb;
}

/***********************************************************
 * ROSA_taskAdd
 *
 * Comment:
 * Add a dynamic task.
 * This function is according to the API given to the students.
 * However, the API lacked some features.
 *
 **********************************************************/
Tcb * ROSA_taskAdd(Tcb * tcbDONTCARE, const char * id, const void * task, int * stackDONTCARE, const int stackSize)
{
	int prio = DEFAULTLOWPRIO;
	return ROSA_taskCreate(id, task, prio, stackSize);
}

/***********************************************************
 * ROSA_taskDestroy
 *
 * Comment:
 * Destroy/Remove a dynamic task.
 * This function destroy the calling task and leave execution
 * to the next task according to the scheduler.
 *
 * Prerequired:
 * This functions should be run in supervisor mode and
 * interrupts turned off.
 **********************************************************/
//TODO: Mechanism for asking any other task to destroy itself.
void _taskDestroy(void)
{
	unsigned int * stack;

	//Find the stack
	stack = EXECTASK->dataarea - EXECTASK->datasize;

	//Free all resources	//TODO: go through semaphore list and free resources.
	free(stack);
	free(EXECTASK);

	//EXECTASK doesnt exist any longer
	EXECTASK = NULL;
	taskState = DESTROYED;

	//Bookkeeping
	_dynTaskNrDec();	//TODO: Remove
}

/*  The old way to kill tasks:

	//Are we removing the first element?
	if(EXECTASK == TCBLIST) {
		TCBLIST = TCBLIST->nexttcb;	//Link out the first element
	}
	else {
		tcb = TCBLIST;
		do {
			tcbtmp = tcb;
			tcb = tcb->nexttcb;
			if(tcb == TCBLIST) {
				while(1);	//Error, could not find EXECTASK in the TCBLIST (we wrapped the search)
			}
		} while(tcb != EXECTASK);

		//Link out the tcb
		tcbtmp->nexttcb = tcb->nexttcb;
	}

	//Find the stack
	stack = tcb->dataarea - tcb->datasize;

	//Free all resources	//TODO: go through semaphore list and free resources.
	free(stack);
	free(tcb);

	//Bookkeeping
	_dynTaskNrDec();
*/

