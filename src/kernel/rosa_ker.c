/*****************************************************************************

                 ///////,   .////    .///' /////,
                ///' ./// ///'///  ///,     '///
               ///////'  ///,///   '/// //;';//,
             ,///' ////,'/////',/////'  /////'/;,

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

#include <stdlib.h>

//Kernel includes
#include "kernel/rosa_def.h"
#include "kernel/rosa_tcb.h"
#include "kernel/rosa_ext.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"
#include "kernel/rosa_dyn.h"
#include "kernel/rosa_scheduler.h"
#include "rosa_config.h"

//Driver includes
#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/pot.h"
#include "drivers/usart.h"


/***********************************************************
 * TCBLIST
 *
 * Comment:
 * 	Global variables that contain the list of TCB's that
 * 	have been installed into the kernel with ROSA_tcbInstall()
 **********************************************************/
Tcb * TCBLIST;

/***********************************************************
 * EXECTASK
 *
 * Comment:
 * 	Global variables that contain the current running TCB.
 **********************************************************/
Tcb * EXECTASK;

//task priority, increments to higher priority for every task installed into the kernel
//This is not really a good way to set priorities. :/
//But since ROSA_tcbCreate() and ROSA_tcbInstall lack prio in the parameter list this simple solution was tested.
//See ROSA_taskCreate() for alternative how it can be done.
static int incrementalPrio = TASKINITIALPRIORITY;
int incrementPrio(void)
{
	return incrementalPrio++;
}

/***********************************************************
 * ROSA_prioSet
 *
 * Comment:
 * Set the priority of a task.
 * Changes the Tcb.
 *
 **********************************************************/
void prioSet(Tcb * tcb, int newPrio)
{
	tcb->prio = newPrio;
}


/*************************************************************
 * The idle task
 ************************************************************/
void idle(void)
{
	ledOn(LED4_GPIO);
	while(1) {
		//Sleep
	}
}

/***********************************************************
 * ROSA_init
 *
 * Comment:
 * 	Initialize the ROSA system
 *
 **********************************************************/
void ROSA_init(void)
{
	//Do initialization of I/O drivers
#if(CONFIG_LED)
	ledInit();									//LEDs
#endif
#if(CONFIG_BUTTON)
	buttonInit();								//Buttons
#endif
#if(CONFIG_JOYSTICK)
	joystickInit();								//Joystick
#endif
#if(CONFIG_POT)
	potInit();									//Potentiometer
#endif
#if(CONFIG_USART0)
	usartInit(USART0, &usart0_options, FOSC0);	//Serial communication
#endif
#if(CONFIG_USART1)
	usartInit(USART1, &usart1_options, FOSC0);	//Serial communication
#endif

	//Start with empty TCBLIST and no EXECTASK.
	TCBLIST = NULL;
	EXECTASK = NULL;

#if(CONFIG_TIMER)
	interruptDisable();
	interruptInit();
	timerInit(PERIODTIME_MS);
#endif

	//Idle task
//	prioSet(ROSA_taskAdd(NULL, "idle", idle, NULL, 50), IDLEPRIORITY);
}

/***********************************************************
 * ROSA_tcbCreate
 *
 * Comment:
 * 	Create the TCB with correct values.
 *
 **********************************************************/
void ROSA_tcbCreate(Tcb * tcb, const char tcbName[CONFIG_NAMESIZE], const void * tcbFunction, unsigned int * tcbStack, const int tcbStackSize)
{
	int i;

	//Initialize the Tcb with the correct values
	//Copy the id/name
	for(i = 0; i < CONFIG_NAMESIZE; i++) {
		tcb->id[i] = tcbName[i];
	}

	//Dont link this TCB anywhere yet.
	tcb->nexttcb = NULL;

	//Set the task function start and return address.
	tcb->staddr = tcbFunction;
	tcb->retaddr = (int)tcbFunction;

	//Set up the stack.
	tcb->datasize = tcbStackSize;
	tcb->dataarea = tcbStack + tcbStackSize;
	tcb->saveusp = tcb->dataarea;

	//Set the initial SR.
	tcb->savesr = ROSA_INITIALSR;

	//Initialize context.
	contextInit(tcb);
}


/***********************************************************
 * ROSA_tcbInstall
 *
 * Comment:
 * 	Install the TCB into the TCBLIST.
 *
 **********************************************************/
void ROSA_tcbInstall(Tcb * tcb)
{
	Tcb * tcbTmp;

	/* Is this the first Tcb installed? */
	if(TCBLIST == NULL) {
		TCBLIST = tcb;
		TCBLIST->nexttcb = tcb;			//Install the first Tcb
		tcb->nexttcb = TCBLIST;			//Make the list circular
	}
	else {
		tcbTmp = TCBLIST;				//Find last Tcb in the list
		while(tcbTmp->nexttcb != TCBLIST) {
			tcbTmp = tcbTmp->nexttcb;
		}
		tcbTmp->nexttcb = tcb;			//Install Tcb last in the list
		tcb->nexttcb = TCBLIST;			//Make the list circular
	}

	//Set the initial priority
	prioSet(tcb, incrementalPrio);
}


