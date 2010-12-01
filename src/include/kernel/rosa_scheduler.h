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

#ifndef _ROSA_SCHEDULER_H_
#define _ROSA_SCHEDULER_H_

#include <stdlib.h>
#include "kernel/rosa_ker.h"
#include "kernel/rosa_systick.h"
#include "system/heap.h"

/***********************************************************
 * Task States
 *
 * Comment:
 * The various states a task can be in.
 **********************************************************/
#define READY 0
#define WAITING 1
#define DESTROYED 2

//The queues for tasks in waiting and ready state.
//~ extern Heap * readyHeap;
//~ extern Heap * waitingHeap;

//Global variable for setting the state of the currently running task.
extern int taskState;
/***********************************************************
 * rrscheduler
 *
 * Comment:
 * 	Minimalistic scheduler for round robin task switch.
 * 	This scheduler choose the next task to execute by looking
 * 	at the nexttcb of the current running task.
 **********************************************************/
void rrscheduler(void);

//Prioscheduler
void scheduler(void);
void prioschedulerInit(void);
int addToScheduler(Tcb * tcb);
int getFromScheduler(Tcb * tcb);
#endif /* _ROSA_SCHEDULER_H_ */
