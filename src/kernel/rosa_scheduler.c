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
//#include "system/list.h"
#include "system/queue.h"

Queue * readyQueue = NULL;
Queue * waitingQueue = NULL;
Queue * prioList = NULL;

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
	EXECTASK = EXECTASK->nexttcb;
}
//~

//~
//~ void schedulerPrintPrioList(void)
//~ {
	//~ Tcb * tmp, * tmp2;
	//~ tmp = TCBLIST;
//~
	//~ if(!prioList) {
		//~ return;
	//~ }
//~
	//~ do {
		//~ tmp2 = (Tcb *) prioList->head->data;
		//~ usartWriteLine(USART, tmp2->id);
//~
//~
	//~ }
//~
//~ }

void schedulderInit(void)
{
	//First make sure the readyQueue exist
	if(!readyQueue) {
		if((readyQueue = malloc(sizeof(Queue)))) {
			listCreate(readyQueue, NULL);
		}
	}
	//Then make sure the waitingQueue exist
	if(!waitingQueue) {
		if((waitingQueue = malloc(sizeof(Queue)))) {
			listCreate(waitingQueue, NULL);
		}
	}

}


void taskWakeUp(Item * task)
{
	//Delete from waitingQueue
	queueRemove(waitingQueue, task);
	//Insert into readyQueue
	queueAdd(readyQueue, task);
	//~ queueSortPrio(readyQueue);	//Not implemented yet.
}

void taskCheckWakeUp()
{
	Queue * queue;
	Item * waitingTask;
	unsigned int now;

	taskWakeUp();
	//First look for any tasks in the waitingQueue that are
	//done waiting and want to run (at thier priority)
	queue = waitingQueue;

	now = ROSA_sysTickGet();

	if(queue) {
		waitingTask = queue->data;
		while(waitingTask) {
			if(waitingTask->data->wakeTime == now) {
				//Wake this task up.
				//Move to readyQueue
				taskWakeUp(waitingTask);
			}

			//Continue with next task in the waitingQueue.
			waitingTask = waitingTask->next;
		}
	}
}

/***********************************************************
 * scheduler
 *
 * Comment:
 * Priority based scheduler.
 *
	//List definition
	typedef struct s_item {
		void * data;
		struct s_item * next;
	} Item;

	typedef struct list_ {
		int size;
		void (*destroy)(void * data);
		Item * head;
		Item * tail;
	} List;
 *
 **********************************************************/
void prioscheduler(void)
{
	taskCheckWakeUp();
}

void oldprioscheduler(void)
{
	Tcb * tmp, * tmp2;
	tmp = TCBLIST;


	//Sort according to the priority
	do {
		tmp2 = (Tcb *)prioList->head->data;
		if(tmp->prio >= tmp2->prio) {
			queueAdd(prioList, tmp);
		}
		else {
			queueAdd(prioList, tmp);
		tmp = tmp->nexttcb;
		}
	} while(tmp != TCBLIST);
	//~ schedulerPrintPrioList();
}
