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
File creation date: 20101109 10:15:24

*/

#ifndef _ROSA_TCB_H_
#define _ROSA_TCB_H_

#include "rosa_config.h"    //CONFIG_NAMESIZE

/***********************************************************
 * TCB block
 *
 * Comment:
 * 	This struct contain all the necessary information to
 * 	do a context switch.
 *
 **********************************************************/
typedef struct Tcbrecord_t {
	//struct Tcbrecord_t * nexttcb;	//Not needed any longer
	char id[CONFIG_NAMESIZE];               //The task id/name
	void (*staddr)(void);                   //Start address
	unsigned int * dataarea;                //The stack data area
	unsigned int datasize;                  //The stack size
	unsigned int * saveusp;                 //The current stack position
	unsigned int SAVER0;                    //Temporary work register
	unsigned int SAVER1;                    //Temporary work register
	unsigned int savesr;                    //The current status register
	unsigned int retaddr;                   //The return address
	unsigned int prio;                      //The task priority
	unsigned int waitUntil;                 //The sysTick on which the task should go from waiting state to ready state
	unsigned int savereg[16];               //The CPU registers
} Tcb;

#endif /*  _ROSA_TCB_H_ */