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

#ifndef _ROSA_KER_H_
#define _ROSA_KER_H_

#include "kernel/rosa_def.h"
#include "kernel/rosa_tcb.h"
#include "rosa_config.h"

/***********************************************************
 * Global variables
 ***********************************************************/
//~ extern Tcb * TCBLIST;
extern Tcb * EXECTASK;


/***********************************************************
 * Kernel low level context switch functions
 ***********************************************************/
extern void ROSA_yield(void);
extern int ROSA_taskDestroy(void);

/***********************************************************
 * Other kernel functions
 ***********************************************************/
//None

/***********************************************************
 * General kernel API
 *
 * Comment:
 *	No comments available.
 *
 **********************************************************/

//Initialize the kernel
void ROSA_init(void);
void ROSA_tcbCreate(Tcb * tcbTask, const char tcbName[CONFIG_NAMESIZE], const void *tcbFunction, void * param, unsigned int * tcbStack, const int tcbStackSize);

//Install a new task TCB into ROSA
//~ extern void ROSA_tcbInstall(Tcb *task); TODO: Remove

//Start running the ROSA kernel
//This start running the created and installed tasks.
extern void ROSA_start(void);

#endif /* _ROSA_KER_H_ */
