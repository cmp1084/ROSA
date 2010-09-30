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

#ifndef rosa_KER_H_
#define rosa_KER_H_

#include "rosa_def.h"

/***********************************************************
 * General kernel API
 *
 * Comment:
 *	No comments available.
 *
 **********************************************************/

//Define the initial value of the satus register
#define ROSA_INITIALSR 0x1c0000

//Timer interrupt service routine
__attribute__((__interrupt__)) extern void ROSA_timerISR(void);

//Kernel YIELD functions
extern void ROSA_yield(void);
extern void ROSA_yieldFromISR(void);

//Initialize the kernel
void ROSA_init(void);
void ROSA_tcbCreate(tcb * tcbTask, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize);

//Install a new task TCB into ROSA
extern void ROSA_tcbInstall(tcb *task);

//Start running the ROSA kernel
//This start running the created and installed tasks.
extern void ROSA_start(void);

#endif /* rosa_KER_H_ */
