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

#ifndef _ROSA_SEMAPHORE_H_
#define _ROSA_SEMAPHORE_H_

/*
File creation date: 20101108 15:41:45

*/

#include "kernel/rosa_def.h"
#include "kernel/rosa_tcb.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_int.h"

/***********************************************************
 * The semaphore functions return OK on success.
 * The semaphore functions return ERROR on failure.
 **********************************************************/

/***********************************************************
 * A LOCKED semaphore is already taken by someone.
 * A UNLOCKED semaphore is free to take.
 **********************************************************/
#define LOCKED 1
#define UNLOCKED 0

typedef struct _sem sem;

struct _sem {
	Tcb * tcbHandle;
	int value;
};

void ROSA_semCreate(sem * semaphore);
int ROSA_semDestroy(sem * semaphore);
void ROSA_semGive(sem * semaphore);
int ROSA_semTake(sem * semaphore);

//TODO: This is bad stuff!
void ROSA_semGiveTo(Tcb * tcbblock, sem * semaphore);

#endif /* _ROSA_SEMAPHORE_H_ */