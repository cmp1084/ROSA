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

//Driver includes
#include "drivers/led.h"
#include "drivers/button.h"
#include "drivers/usart.h"
#include "drivers/delay.h"
#include "drivers/pot.h"

//Include configuration
#include "rosa_config.h"

//Data blocks for the tasks
#define T1_STACK_SIZE 0x40
static int t1_stack[T1_STACK_SIZE];
static tcb t1_tcb;

#define T2_STACK_SIZE 0x40
static int t2_stack[T2_STACK_SIZE];
static tcb t2_tcb;

//TASK1
//LED4 goes dark
//LED6 lights up
void task1(void)
{
	//~ int now = ROSA_timerGetSysTick();
	while(1) {
		ledOn(LED0_GPIO);
		ledOff(LED1_GPIO);
		//delay_ms(potGetValue());
		ROSA_timerPeriodSet(potGetValue());
		//ROSA_contextSwitch();
	}
}

//TASK2
//LED4 lights up
//LED6 goes dark
void task2(void)
{
	//~ int now = ROSA_timerGetSysTick();
	while(1) {
		ledOff(LED0_GPIO);
		ledOn(LED1_GPIO);
		delay_ms(500);
		//waitUntil(&now, 10);
		ROSA_contextSwitch();
	}
}

int main(void)
{
	//Initialize the ROSA kernel
	ROSA_init();

	//Create tasks and install them into the ROSA kernel
	ROSA_tcbCreate(&t1_tcb, "tsk1", task1, t1_stack, T1_STACK_SIZE);
	ROSA_tcbInstall(&t1_tcb);
	ROSA_tcbCreate(&t2_tcb, "tsk2", task2, t2_stack, T2_STACK_SIZE);
	ROSA_tcbInstall(&t2_tcb);

	//Start the ROSA kernel
	ROSA_start();
	/* Execution will never return here */
	while(1);
}
