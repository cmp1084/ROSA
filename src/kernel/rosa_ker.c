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

#include "kernel/rosa_def.h"
#include "kernel/rosa_ext.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"
#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/usart.h"

/***********************************************************
 * TCBLIST
 *
 * Comment:
 * 	Global variables that contain the list of TCB's that
 * 	have been installed into the kernel with ROSA_tcbInstall()
 **********************************************************/
tcb * TCBLIST;

/***********************************************************
 * EXECTASK
 *
 * Comment:
 * 	Global variables that contain the current running TCB.
 **********************************************************/
tcb * EXECTASK;


/***********************************************************
 * ROSA_scheduler
 *
 * Comment:
 * 	Minimalistic scheduler for round robin task switch.
 * 	This scheduler choose the next task to execute by looking
 * 	at the nexttcb of the current running task.
 **********************************************************/
void ROSA_scheduler(void)
{
	EXECTASK = EXECTASK->nexttcb;
}


/***********************************************************
 * timerInterruptHandler
 *
 * Comment:
 * 	This is the basic timer interrupt service routine.
 **********************************************************/
__attribute__((__interrupt__)) void ROSA_timerISR(void)
{
	int sr;
	volatile avr32_tc_t * tc = &AVR32_TC;

	//Read the timer status register to determine if this is a valid interrupt
	sr = tc->channel[0].sr;
	if(sr & AVR32_TC_CPCS_MASK) {
		ROSA_yieldFromISR();
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
	// USART options.
	static const usart_options_t usart_options =
	{
		.baudrate    = 57600,
		.charlength  = 8,
		.paritytype  = USART_NO_PARITY,
		.stopbits    = USART_1_STOPBIT,
		.channelmode = USART_NORMAL_CHMODE
	};

	TCBLIST = NULL;
	EXECTASK = NULL;

	//Do I/O driver initializations
	ledInit();									//LEDs
	buttonInit();								//Buttons
	joystickInit();								//Joystick
	potInit();									//Potentiometer
	usartInit(USART, &usart_options, FOSC0);	//Serial communication

	//Set up interrupts
	ROSA_interruptDisable();
	ROSA_interruptInit();
	ROSA_interruptEnable();

	//Set up the timer, but do not start it yet.
	//We are not ready to take care of timer interrupt.
	ROSA_timerInit();

	//Additional low level initialization
	ROSA_init_lowlevel();
}


/***********************************************************
 * ROSA_tcbCreate
 *
 * Comment:
 * 	Create the TCB with correct values.
 *
 **********************************************************/
void ROSA_tcbCreate(tcb * tcbTask, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize)
{
	int i;

	//Initialize the tcb with the correct values
	for(i = 0; i < NAMESIZE; i++) {
		tcbTask->id[i] = tcbName[i];		//Copy the id/name
	}

	//Dont link this TCB anywhere yet
	tcbTask->nexttcb = NULL;

	//Set the task function
	tcbTask->staddr = tcbFunction;
	tcbTask->retaddr = (int)tcbFunction;

	//Set up the stack
	tcbTask->datasize = tcbStackSize;
	tcbTask->dataarea = tcbStack + tcbStackSize;
	tcbTask->saveusp = tcbTask->dataarea;

	//Set the initial SR
	tcbTask->savesr = ROSA_INITIALSR;

	//Initialize context
	ROSA_contextInit(tcbTask);
}


/***********************************************************
 * ROSA_tcbInstall
 *
 * Comment:
 * 	Install the TCB into the TCBLIST.
 *
 **********************************************************/
void ROSA_tcbInstall(tcb * tcbTask)
{
	tcb * tcbTmp;

	/* Is this the first tcb installed? */
	if(TCBLIST == NULL) {
		TCBLIST = tcbTask;
		TCBLIST->nexttcb = tcbTask;			//Install the first tcb
		tcbTask->nexttcb = TCBLIST;			//Make the list circular
	}
	else {
		tcbTmp = TCBLIST;					//Find last tcb in the list
		while(tcbTmp->nexttcb != TCBLIST) {
			tcbTmp = tcbTmp->nexttcb;
		}
		tcbTmp->nexttcb = tcbTask;			//Install tcb last in the list
		tcbTask->nexttcb = TCBLIST;			//Make the list circular
	}
}


/***********************************************************
 * ROSA_start
 *
 * Comment:
 * 	Start up the kernel.
 * 	Last in this function a call to ROSA_start_lowlevel is done.
 * 	This does the initial context switch to the first task in
 * 	the TCBLIST.
 *
 **********************************************************/
void ROSA_start(void)
{
	//EXECTASK is the first task to start.
	EXECTASK = TCBLIST;
	ROSA_start_lowlevel();
	//Execution will NOT proceed here!
}


/***********************************************************
 * ROSA_yield
 *
 * Comment:
 * 	Perform a voluntary yield from a task.
 *
 **********************************************************/
void ROSA_yield(void)
{
	//Context save need to be done first!
	ROSA_contextSave();			//Save context for current task
	ROSA_scheduler();			//Find next task to execute
	ROSA_contextRestore();		//Restore context for next task, switches interrupt back on, and start the timer again
	//Execution will NOT proceed here!
}


/***********************************************************
 * ROSA_yieldFromISR
 *
 * Comment:
 * 	Perform a yield from an ISR
 *
 **********************************************************/
void ROSA_yieldFromISR(void)
{
	ROSA_contextSaveFromISR();	//Switch the task context
	ROSA_scheduler();			//Find next task to execute
	//The contextRestoreFromInterrupt is preferably done last.
	ROSA_contextRestoreFromISR();
}
