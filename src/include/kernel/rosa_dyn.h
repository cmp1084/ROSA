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

#ifndef _ROSA_DYN_H_
#define _ROSA_DYN_H_

#include "kernel/rosa_tcb.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_scheduler.h"
#include "app/warning.h"

//Create a dynamic task and install it into the ROSA kernel.
Tcb * ROSA_taskAdd(Tcb * TCB, const char * id, const 	void * taskFunction, int * stack, const int stackSize);

//Create a dynamic task and install it into the ROSA kernel. Alternative function, shorter parameter list.
Tcb * ROSA_taskCreate(const char * id, const void * taskFunction, const int prio, const int stackSize);

//Bookkeeping function, returns the number of dynamic tasks in the system
int ROSA_dynTaskNrGet(void);

#endif /* _ROSA_DYN_H_ */