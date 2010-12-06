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
#include "drivers/spi.h"

//Include configuration
#include "rosa_config.h"

//Include system files
#include "system/semaphore.h"

#define OUTPUTTIME 0x100


void stat2(void);

void stat3(void);

/*************************************************************
 * Semaphores
 ************************************************************/
sem sem_test;
sem * sem_test2;
sem * sem_usart;

//Data blocks for the tasks
//~ #define T1_STACK_SIZE 0x40
//~ static int t1_stack[T1_STACK_SIZE];
//~ static Tcb t1_tcb;

//~ #define T2_STACK_SIZE 0x40
//~ static int t2_stack[T2_STACK_SIZE];
//~ static Tcb t2_tcb;


__attribute__((__naked__))
void opttask1(void)
{
	ROSA_wait(250);
	//~ delay_ms(125/4);
	while(1) {
		ledToggle(LED0_GPIO);
		ROSA_wait(501);
		//~ delay_ms(251/4);
		ROSA_yield();
	}
}

__attribute__((__naked__))
void opttask2(void)
{
	while(1) {
		ledToggle(LED1_GPIO);
		ROSA_wait(500);
		//~ delay_ms(250/4);
		ROSA_yield();
	}
}

/*************************************************************
 * Task1
 * LED0 lights up
 * LED1 goes dark
 ************************************************************/
__attribute__((__naked__)) void task1(void)
{
	ROSA_wait(50);
	while(1) {
		ledOn(LED0_GPIO);
		ledOff(LED1_GPIO);
		ROSA_wait(101);
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
		ROSA_wait(100);
	}
}

/*************************************************************
 * Task3
 *************************************************************/
void task3(void)
{
	while(1) {
		ledToggle(LED2_GPIO);

		if(isButton(PUSH_BUTTON_0)) {
			ROSA_taskCreate("---K", stat2, 2, 0x40);
			ledToggle(LED3_GPIO);
		}
		if(isButton(PUSH_BUTTON_1)) {
			ROSA_taskCreate("---C", stat2, 2, 0x40);
			ROSA_taskCreate("dynD", stat3, 1, 0x40);
		}
		ROSA_wait(52);
	}
}



extern int _SSPGet(void);
extern int _USPGet(void);
void printStatus(void)
{
	int usp, now;

	//Wait until USART is free
	while(!ROSA_semTake(sem_usart)) {
		now = ROSA_sysTickGet();
		EXECTASK->waitUntil = now + 1;	//+2 since sysTick+1 at next contextSwitch, and we want to sleep atleast 1 tick, hence +1+1.
		taskState = WAITING;
		ROSA_yield();
	}

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

	usartWriteLine(USART0, "\n");
	//Give the USART back
	ROSA_semGive(sem_usart);

	//ledOff(LED3_GPIO);
	ROSA_taskDestroy();
}

void stat(void)
{
	while(1) {
		ROSA_taskCreate("prin", printStatus, 1, 0x40);
		ROSA_wait(OUTPUTTIME);
	}
}


void stat2(void)
{
	char taskname[] = "dyn-\n";
	int now;

	while(1) {
		while(!ROSA_semTake(sem_usart)) {
		now = ROSA_sysTickGet();
		EXECTASK->waitUntil = now + 1;
		taskState = WAITING;
			ROSA_yield();
		}
		taskname[3] = EXECTASK->id[3];
		usartWriteLine(USART0, taskname);
		ROSA_semGive(sem_usart);
		ROSA_wait(1000);
		ROSA_taskDestroy();
	}
}


void stat3(void)
{
	char taskname[] = "dyn-\n";
	int now;

	while(1) {
		while(!ROSA_semTake(sem_usart)) {
		now = ROSA_sysTickGet();
		EXECTASK->waitUntil = now + 1;
		taskState = WAITING;
			ROSA_yield();
		};
		taskname[3] = EXECTASK->id[3];
		usartWriteLine(USART0, taskname);
		ROSA_semGive(sem_usart);
		ROSA_wait(50);
		if(isButton(PUSH_BUTTON_2)) {
			ROSA_taskDestroy();
			ROSA_wait(50);
		}
		ROSA_wait(50);
	}
}

/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{
	//Initialize the ROSA kernel
	ROSA_init();
	spiEnable(&AVR32_SPI1);
	//~ usartWriteLine(USART0, "\e[2JROSA starting...\n"); //TODO
	usartWriteLine(USART0, "\nROSA starting...\n");

	//Create tasks and install them into the ROSA kernel
	//The old way is no longer needed
	//~ ROSA_tcbCreate(&t1_tcb, "tsk1", task1, t1_stack, T1_STACK_SIZE);
	//~ ROSA_tcbInstall(&t1_tcb);
	//~ ROSA_tcbCreate(&t2_tcb, "tsk2", task2, t2_stack, T2_STACK_SIZE);
	//~ ROSA_tcbInstall(&t2_tcb);
	//~ ...
	//The new way to create tasks.
	//I left the old API since this way of creating tasks are much sweeter.
	//ROSA_taskCreate(char * id, void * taskFunc, int prio, int stackSize);
	//~ ROSA_taskCreate("tsk1", task1, 1, 0x40);
	//~ ROSA_taskCreate("tsk2", task2, 2, 0x40);
	ROSA_taskCreate("tsk3", task3, 3, 0x40);
	ROSA_taskCreate("stat", stat, 6, 0x40);

	ROSA_taskCreate("tsk1", opttask1, 1, 0x40);
	ROSA_taskCreate("tsk2", opttask2, 1, 0x40);

	//~ sem_test2 = malloc(sizeof(sem));
	//Create semaphores
	//~ ROSA_semCreateGlobal(&sem_test);
	//~ ROSA_semCreate(&sem_test2);
	ROSA_semCreate(&sem_usart);

	//Start the ROSA kernel
	ROSA_start();
	/* Execution will never return here */
	while(1);
}
