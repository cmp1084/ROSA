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

Heap * waitingHeap = NULL;
Heap * readyHeap = NULL;

int moveTaskToWaitingHeap = FALSE;
/***********************************************************
 * scheduler
 *
 * Comment:
 * 	Minimalistic scheduler for round robin task switch.
 * 	This scheduler choose the next task to execute by looking
 * 	at the nexttcb of the current running task.
 **********************************************************/
void scheduler(void)
{
	prioscheduler();
	return;
	//Find the next task to execute
	//EXECTASK = EXECTASK->nexttcb;
}


int waitingcmp(const void * key1, const void * key2)
{
	int waitUntil1 = ((Tcb *)key1)->waitUntil;
	int waitUntil2 = ((Tcb *)key2)->waitUntil;
	if(waitUntil1 > waitUntil2) return -1;
	if(waitUntil1 == waitUntil2) return 0;
	return 1;
}

int readycmp(const void * key1, const void * key2)
{
	int prio1 = ((Tcb *)key1)->prio;
	int prio2 = ((Tcb *)key2)->prio;
	if(prio1 > prio2) return 1;
	if(prio1 == prio2) return 0;
	return -1;
}

void prioschedulerInit(void)
{
	waitingHeap = malloc(sizeof(Heap));
	readyHeap = malloc(sizeof(Heap));
	if(!waitingHeap && !readyHeap) {
		while(1); //Error
	}
	heapInit(waitingHeap, &waitingcmp, NULL);
	heapInit(readyHeap, &readycmp, NULL);
}

void moveToReadyHeap(void)
{
	Tcb * tcb;
	heapExtract(waitingHeap, (void **)&tcb);
	heapInsert(readyHeap, tcb);
}

void prioscheduler(void)
{
	unsigned int now;

	//Look for waiting tasks that are ready to be moved to ready queue
	now = ROSA_sysTickGet();
	while(waitingHeap->tree) {
		if(((Tcb *)heapPeek(waitingHeap))->waitUntil <= now) {
			moveToReadyHeap();
		}
		else {
			break;
		}
	}

	//If moveTaskToWaitingHeap = FALSE a task go to readyHeap at task switch
	//If moveTaskToWaitingHeap = TRUE a task go to waitingHeap at task switch
	//If moveTaskToWaitingHeap = DESTROYED a task doesnt go to any state at all.
	switch(moveTaskToWaitingHeap) {
		case FALSE:
			if(heapInsert(readyHeap, EXECTASK) != 0) {
				while(1); //Error
			}
			break;
		case TRUE:
			if(heapInsert(waitingHeap, EXECTASK) != 0) {
				while(1); //Error
			}
			moveTaskToWaitingHeap = FALSE;
			break;
		case DESTROYED:
			moveTaskToWaitingHeap = FALSE;
			break;
		default:
			moveTaskToWaitingHeap = FALSE;
			break;
	}
	if(heapExtract(readyHeap, (void **)&EXECTASK) != 0) {
		//EXECTASK = idle;
	}
}

