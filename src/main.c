/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// ///''\\,
             ,///' '///,'/////',/////'  /////'\\,

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

//Standard library includes
#include <avr32/io.h>
#include <stdlib.h>

//Kernel includes
#include "kernel/rosa_ker.h"
#include "kernel/rosa_dyn.h"
#include "kernel/rosa_systick.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"
#include "drivers/button.h"

//Include configuration
#include "rosa_config.h"

//Include system files
#include "system/semaphore.h"

//Include application files
#include "app/warning.h"

#define OUTPUTTIME 0x10


void stat2(void);

void stat3(void);

/*************************************************************
 * Semaphores
 ************************************************************/
sem sem_test;
sem sem_usart;

//Data blocks for the tasks
#define T1_STACK_SIZE 0x40
//~ static int t1_stack[T1_STACK_SIZE];
//~ static Tcb t1_tcb;

#define T2_STACK_SIZE 0x40
//~ static int t2_stack[T2_STACK_SIZE];
//~ static Tcb t2_tcb;

/*************************************************************
 * Task1
 * LED0 lights up
 * LED1 goes dark
 ************************************************************/
void task1(void)
{
	ROSA_wait(500);
	while(1) {
		ledOn(LED0_GPIO);
		ledOff(LED1_GPIO);
		ROSA_wait(1001);
	}
}

/*************************************************************
 * Task2
 *************************************************************/
void task2(void)
{
	while(1) {
		ledOff(LED0_GPIO);
		ledOn(LED1_GPIO);
		ROSA_wait(1000);
	}
}

/*************************************************************
 * Task3
 *************************************************************/
void task3(void)
{
	while(1) {
		ledToggle(LED2_GPIO);
		//~ ledOff(LED0_GPIO);
		//~ ledOff(LED1_GPIO);

		if(isButton(PUSH_BUTTON_0)) {
			//ROSA_taskCreate("but0", but0, 2, 0x40);
			ROSA_taskCreate("---K", stat2, 2, 0x30);
			ledToggle(LED3_GPIO);
		}
		if(isButton(PUSH_BUTTON_1)) {
			ROSA_taskCreate("---C", stat2, 2, 0x30);
			ROSA_taskCreate("dynD", stat3, 2, 0x30);
			//~ ROSA_taskCreate("dynE", stat2, 2, 0x10);
			//~ ROSA_taskCreate("dynF", stat2, 2, 0x10);
			//~ ROSA_taskCreate("dynG", stat3, 2, 0x10);
			//~ ROSA_taskCreate("dynH", stat3, 2, 0x10);
			//ROSA_taskCreate("dynI", stat3, 2, 0x10);
		}
		ROSA_wait(1002);
	}
}



extern int _SSPGet(void);
extern int _USPGet(void);
void printStatus(void)
{
	int usp;

	//Wait until USART is free
	while(!ROSA_semTake(&sem_usart));

	//usartWriteLine(USART, "\f");
	usartWriteLine(USART0, "\e[0;0H");
	usartWriteLine(USART0, "SysTick: ");
	usartWriteValue(USART0, ROSA_sysTickGet());

	usartWriteLine(USART0, "\n# tasks: ");
	usartWriteValue(USART0, ROSA_dynTaskNrGet());

	usp = _USPGet();
	usartWriteLine(USART0, "\nSSP: ");
	usartWriteValue(USART0, _SSPGet());
	usartWriteLine(USART0, "\nUSP: ");
	usartWriteValue(USART0, usp);

	//~ usartWriteLine(USART0, "\n\n");
	//Give the USART back
	ROSA_semGive(&sem_usart);

	//ledOff(LED3_GPIO);
	ROSA_taskDestroy();
	//~ while(1);	//TODO: Does this help?
}

void stat(void)
{
	while(1) {
		//while(!ROSA_semTake(&sem_usart));
		ROSA_taskCreate("prin", printStatus, 1, 0x40);
		//ROSA_semGive(&sem_usart);
		ROSA_wait(OUTPUTTIME);
	}
}


void stat2(void)
{
	char taskname[] = "dyn-\n";

	while(1) {
		while(!ROSA_semTake(&sem_usart));
		taskname[3] = EXECTASK->id[3];
		usartWriteLine(USART0, taskname);
		ROSA_semGive(&sem_usart);
		ROSA_wait(10000);
		ROSA_taskDestroy();
	}
}


void stat3(void)
{
	char taskname[] = "dyn-\n";

	while(1) {
		//while(!ROSA_semTake(&sem_usart));
		//ROSA_taskAdd(NULL, "dyns", printStatus, NULL, 40);	//This task will not persist
		taskname[3] = EXECTASK->id[3];
		//usartWriteLine(USART0, taskname);
		//ROSA_semGive(&sem_usart);
		ROSA_wait(500);
		if(isButton(PUSH_BUTTON_2)) {
			ROSA_taskDestroy();
			ROSA_wait(500);
		}
	}
}

/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{

	//Initialize the ROSA kernel
	ROSA_init();
	//~ warningInit();
	//ledOn(LED6_GPIO);
	//~ usartWriteLine(USART0, "\e[2JROSA starting...\n");
	usartWriteLine(USART0, "\nROSA starting...\n");

	//Create tasks and install them into the ROSA kernel
	//~ ROSA_tcbCreate(&t1_tcb, "tsk1", task1, t1_stack, T1_STACK_SIZE);
	//~ ROSA_tcbInstall(&t1_tcb);
	//~ ROSA_tcbCreate(&t2_tcb, "tsk2", task2, t2_stack, T2_STACK_SIZE);
	//~ ROSA_tcbInstall(&t2_tcb);

	ROSA_taskCreate("tsk1", task1, 2, 0x40);
	ROSA_taskCreate("tsk2", task2, 3, 0x40);
	ROSA_taskCreate("tsk3", task3, 4, 0x40);
	ROSA_taskCreate("stat", stat, 6, 0x40);
	//~ ROSA_taskCreate("tskC", stat, 2, 0x40);
	//ROSA_taskAdd(NULL, "dyns", stat2, NULL, 0x40);	//This task will not persist
	//~ ROSA_taskCreate("dynC", stat2, 2, 0x40);
	//~ ROSA_taskCreate("dynD", stat2, 2, 0x40);
	//~ ROSA_taskCreate("dynE", stat2, 2, 0x40);
	//~ ROSA_taskCreate("dynF", stat2, 2, 0x40);

	//It would have been sweeter with Tcb * ROSA_taskAdd(char * name, void * func, int stackSize); or passing params
	//~ ROSA_taskAdd(NULL, "dyn0", dyntask0, NULL, 40);	//This task will destroy itself
	//~ ROSA_taskAdd(NULL, "dyn1", task3, NULL, 40);	//This task will persist
	//~ ROSA_taskAdd(NULL, "dyn2", task4, NULL, 40);	//This task will destroy itself, buy be created again by task3
	//~ ROSA_taskAdd(NULL, "stat", stat, NULL, 40);	//This task will persist

	//Create sem_test semaphore
	//~ ROSA_semCreate(&sem_test);
	ROSA_semCreate(&sem_usart);


	//Start the ROSA kernel
	ROSA_start();
	/* Execution will never return here */
	while(1);
}
