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

//Kernel includes
#include "kernel/rosa_ker.h"
#include "kernel/rosa_dyn.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"

//Include configuration
#include "rosa_config.h"

//Include system files
#include "system/semaphore.h"

/*************************************************************
 * Semaphores
 ************************************************************/
sem sem_test;
sem sem_usart;

//Data blocks for the tasks
#define T1_STACK_SIZE 0x40
static int t1_stack[T1_STACK_SIZE];
static tcb t1_tcb;

#define T2_STACK_SIZE 0x40
static int t2_stack[T2_STACK_SIZE];
static tcb t2_tcb;

/*************************************************************
 * Task1
 * LED0 lights up
 * LED1 goes dark
 ************************************************************/
void task1(void)
{
	while(1) {
		ledOn(LED0_GPIO);
		ledOff(LED1_GPIO);
		while(ROSA_semTake(&sem_test) != OK) {
			ledOn(LED2_GPIO);
		}
		ledOff(LED2_GPIO);
		delay_ms(300);
		ROSA_semGive(&sem_test);
		delay_ms(250);
		if(ROSA_semTake(&sem_usart)) {
			usartWriteLine(USART, "task1\n");
			ROSA_semGive(&sem_usart);
		}
		ROSA_yield();
	}
}

/*************************************************************
 * Task2
 * LED0 goes dark
 * LED1 lights up
 ************************************************************/
void task2(void)
{
	while(1) {
		ledOff(LED0_GPIO);
		ledOn(LED1_GPIO);
		while(ROSA_semTake(&sem_test) != OK) {
			ledOn(LED3_GPIO);
		}
		ledOff(LED3_GPIO);
		delay_ms(250);
		ROSA_semGive(&sem_test);
		if(ROSA_semTake(&sem_usart)) {
			usartWriteLine(USART, "task2\n");
			ROSA_semGive(&sem_usart);
		}
		ROSA_yield();
	}
}

void dyntask0(void)
{
	while(1) {
		while(!ROSA_semTake(&sem_usart));
		usartWriteLine(USART, "ROSA dyntask0");
		delay_ms(1000);
		ROSA_semGive(&sem_usart);
		ROSA_taskDestroy();
	}
}

tcb * dyntcb0;

/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{
	//Initialize the ROSA kernel
	ROSA_init();

	usartWriteLine(USART, "\nROSA start\n");
	//Create tasks and install them into the ROSA kernel
	ROSA_tcbCreate(&t1_tcb, "tsk1", task1, t1_stack, T1_STACK_SIZE);
	ROSA_tcbInstall(&t1_tcb);
	ROSA_tcbCreate(&t2_tcb, "tsk2", task2, t2_stack, T2_STACK_SIZE);
	ROSA_tcbInstall(&t2_tcb);

	ROSA_taskAdd(dyntcb0, "dyn0", dyntask0, NULL, 10);

	//Create sem_test semaphore
	ROSA_semCreate(&sem_test);
	ROSA_semCreate(&sem_usart);


	//Start the ROSA kernel
	ROSA_start();
	/* Execution will never return here */
	while(1);
}
