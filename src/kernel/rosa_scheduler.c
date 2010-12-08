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

#include "kernel/rosa_scheduler.h"
#include "kernel/rosa_tim.h"
#include "drivers/usart.h"

//The queues for tasks in waiting and ready state.
static Heap * waitingHeap = NULL;
static Heap * readyHeap = NULL;

//Global variable for setting the state of the currently running task.
int taskState = READY;

int addToScheduler(Tcb * tcb)
{
	return heapInsert(readyHeap, tcb);
}

int getFromScheduler(Tcb ** tcb)
{
	return heapExtract(readyHeap, (void *)tcb);
}

/***********************************************************
 * Functions for prioscheduler
 *
 * Comment:
 * Priority scheduler
 * This scheduler choose the next task to execute by
 * extracting the task with the higest priority from the
 * ready heap.
 **********************************************************/
static int waitingcmp(const void * key1, const void * key2)
{
	int waitUntil1 = ((Tcb *)key1)->waitUntil;
	int waitUntil2 = ((Tcb *)key2)->waitUntil;
	if(waitUntil1 > waitUntil2) return -1;
	if(waitUntil1 == waitUntil2) return 0;
	return 1;
}

static int readycmp(const void * key1, const void * key2)
{
	int prio1 = ((Tcb *)key1)->prio;
	int prio2 = ((Tcb *)key2)->prio;
	if(prio1 > prio2) return 1;
	if(prio1 == prio2) return 0;
	return -1;
}

void schedulerInit(void)
{
	waitingHeap = (Heap *)malloc(sizeof(Heap));
	readyHeap = (Heap *)malloc(sizeof(Heap));
	if(!waitingHeap && !readyHeap) {
		while(1); //Error
	}
	heapInit(waitingHeap, &waitingcmp, NULL);
	heapInit(readyHeap, &readycmp, NULL);
}

static void moveToReadyHeap(void)
{
	Tcb * tcb;
	heapExtract(waitingHeap, (void **)&tcb);
	heapInsert(readyHeap, tcb);
}

void scheduler(void)
{
	unsigned int now;

	//Do various operations depending on the task state
	switch(taskState) {
		case READY:
			if(heapInsert(readyHeap, EXECTASK) != 0) {
				while(1); //Error (probably out of mem)
			}
			break;
		case WAITING:
			if(heapInsert(waitingHeap, EXECTASK) != 0) {
				while(1); //Error (probably out of mem)
			}
			taskState = READY;	//Go back to the default state = READY
			break;
		case DESTROYED:
			//Do not move the previously running task back to any state
			//as it already have been destroyed.
			taskState = READY;	//Go back to the default state
			break;
		default:
			while(1);	//Error
			break;
	}

	//Look for waiting tasks that are ready to be moved to ready state
	now = ROSA_sysTickGet();
	while(waitingHeap->tree) {
		if(((Tcb *)heapPeek(waitingHeap))->waitUntil <= now) {
			moveToReadyHeap();
		}
		else {
			break;
		}
	}
	heapExtract(readyHeap, (void **)&EXECTASK);

	//~ extern void idle(void); TODO:
	//~ //Print debug info
	//~ if(EXECTASK->staddr != idle) {
		//~ usartWriteValue(USART0, ROSA_sysTickGet());
		//~ usartWriteLine(USART0, ": ");
		//~ usartWriteLine(USART0, EXECTASK->id);
		//~ usartWriteLine(USART0, "\n");
	//~ }


}

