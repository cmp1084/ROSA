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
 * rosa_ker_asm.S
 *
 *  Created on: Sep 6, 2010
 *      Author: marcus
 */

#include <avr32/io.h>
#include "kernel/rosa_off.i"
#include "rosa_config.h"

	//Put the code and data in the executable .text section
	.section .text, "ax", @progbits

	//Global Kernel functions
	.global contextInit
	.global ROSA_yield
	.global ROSA_taskDestroy
	.global _SSPGet		//TODO: Remove
	.global _USPGet
	.global _ROSA_start	//Should only be accessed from ROSA_start() in rosa_ker.c
/*********************************************************
* ROSA_start kernel code
*
* Comment:
* Start the kernel.
* Do the initial context switch to the first task in
* the TCBLIST.
*
*********************************************************/
_ROSA_start:
	lda.w r12,EXECTASK
	ld.w r12,r12[0]

	//Set up start USP
	//~ ld.w r0,r12[TCB.SAVEUSP]
	ld.w r0,r12[TCB.SAVEREG.SP]
	st.w --sp,r0
	ldmts sp,sp
	ld.w r0,sp++

	//Load start lr, execution will jump here later
	ld.w lr,r12[TCB.SAVEREG.PC]
	//~ ld.w lr,r12[TCB.STADDR]

	//Set up start SR, enter user mode
	ld.w r0,r12[TCB.SAVESR]
	mtsr 0x0,r0

	//Load start registers
	//~ ld.w r0,r12[TCB.SAVEREG.R0]
	//~ ...
	ld.w r12,r12[TCB.SAVEREG.R12]

#if(CONFIG_TIMER)
	pushm lr
	call timerStart
	call interruptEnable
	popm lr
#endif

	mov pc,lr


/*********************************************************
 * contextInit
 *
 * Comment:
 * The initial save of a tasks context to the Tcb
 * from supervisor mode.
 *
 * Parameters:
 * In: r12 = Tcb * tcb, pointer to the TCB block
 *     r11 = void * param, the task parameters
 ********************************************************/
contextInit:
	pushm r11

	//Put parameters in r12
	st.w r12[TCB.SAVEREG.R12],r11

	//Initialize lr in the savereg area
	ld.w r11,r12[TCB.STADDR]			//TODO: Remove the need for STADDR, use SAVEREG.PC directly in ker.c However, STADDR might make debugging a little easier for reqognising functions (well, id makes it easy enough)
	st.w r12[TCB.SAVEREG.PC],r11
	st.w r12[TCB.SAVEREG.LR],r11

	//Initialize the USP
	ld.w r11,r12[TCB.DATAAREA]
	st.w r12[TCB.SAVEREG.SP],r11

	//Initialize regs to zero
	//~ mov r11,0x0
	//~ st.w r12[TCB.SAVEREG.R0],r11
	//~ ...
	//~ st.w r12[TCB.SAVEREG.R12],r11

	popm r11
	mov pc,lr


/*********************************************************
* ROSA_yield kernel code, various functions:
* 	contextSave
* 	contextRestore
*
* Comment:
* 	Task switch routines.
* 	These routines are executed after a call to
* 	ROSA_yield() or ROSA_yieldFromISR().
*
*********************************************************/
//This is the current state of the supervisor and user stack frame:
//
//This show only the top elements of the supervisor and user stack
//we are interest in.
//
//Stack frame, supervisor stack:
//------------------------------
// * SR, The saved SR is at the top of the supervisor stack.
//
// * PC, The PC of the next user instruction comes next. The 'rets'
//       instruction will continue execution at this address.

//Stack frame, user stack:
//------------------------
// * LR_TASK, The PC of the next user instruction.
//

//SF = Stack Frame

//Supervisor stack frame
.equ SF_SR, 0x00	//The SR is at the top of the supervisor stack.
.equ SF_PC, 0x04	//The PC of the next user instruction is located here.

//User stack frame
.equ SF_LR_TASK, 0x00 //The link back to the next user task instruction is located here.



//contextSave:
//	pushm r12
//
//	//Fetch the current executing task
//	lda.w r12,EXECTASK
//	ld.w r12,r12[0x0]

//	//Save work registers to TCB
//	st.w r12[TCB.SAVEREG.R0],r0
//	//~ st.w r12[TCB.SAVEREG.R1],r1
//	ld.w r0,sp++					//Use r0 to save r12
//	st.w r12[TCB.SAVEREG.R12],r0

//	//Save task SR to TCB
//	ld.w r0,sp[SF_SR]
//	st.w r12[TCB.SAVESR],r0
//
//	//Save task registers r1-r11 to TCB
//	//~ mov r0,TCB.SAVEREG.R1
//	//~ add r0,r12
//	sub r12,-TCB.SAVEREG.R1
//	stmts r0,r1-r11
//
//	//Get the address of the USP
//	mov r0,sp
//	st.w --sp,r0
//	stmts sp,sp
//	ld.w r1,sp++					//USP in r1
//
//	//Save RETADDR to TCB
//	ld.w r0,r1[SF_LR_TASK]			//Get lr from user stack, we want to return to task, not to contextSwitch
//	st.w r12[TCB.RETADDR],r0		//correction of stack is done later
//
//	//Save LR_task
//	st.w r12[TCB.SAVEREG.LR],r0
//
//	//Correct the USP for the ROSA_yield() call.
//	//This is done since we do not want to return to ROSA_yield(),
//	//where the stack frame is currently at, but to the executing task.
//	sub r1,-0x04
//
//	//Save USP
//	st.w r12[TCB.SAVEUSP],r1
//
//	mov pc,lr
/*********************************************************
* contextSave
*
* Comment:
* Task switch routine. Executed only in supervisor mode.
* This routine save task context.
* Context of the TCB referenced by EXECTASK will be saved.
*
*********************************************************/
contextSave:
	pushm r12

	//Fetch the current executing task
	lda.w r12,EXECTASK
	ld.w r12,r12[0x0]

	//Save task registers r0-sp to TCB
	sub r12,-TCB.SAVEREG.SP
	stmts r12,r0-sp					//R12 is not correct saved at this point (EXECTASK is saved as R12!)

	//Fetch the current executing task (again!) //TODO: can we sub r11,r12,-TCB.SAVEREG.SP or similar to avoid this? We can use lr
	lda.w r12,EXECTASK
	ld.w r12,r12[0x0]

	popm r11
	st.w r12[TCB.SAVEREG.R12],r11	//Use R11 to save the correct R12

	//save LR from user stack
	ld.w r11,r12[TCB.SAVEREG.SP]

	//save LR	//This LR is pointing to the instruction after the ROSA_yield() call in the task
	ld.w r10,r11[0x0]
	st.w r12[TCB.SAVEREG.LR],r10
	st.w r12[TCB.SAVEREG.PC],r10	//In this case, where a manual yield have been done, PC is the same as LR

	//Correct the user stack pointer
	sub r11,-4
	st.w r12[TCB.SAVEREG.SP],r11

	//Save task SR to TCB
	ld.w r11,sp[SF_SR]
	st.w r12[TCB.SAVESR],r11

	mov pc,lr


/*********************************************************
* contextRestore
*
* Comment:
* Taskswitch routine. This routine restore task context.
* Context of the TCB referenced by EXECTASK will be restored.
*
*********************************************************/
contextRestore:
	//Load the new EXECTASK
	lda.w r12,EXECTASK
	ld.w r12,r12[0x0]

	//restore SR
	ld.w r11,r12[TCB.SAVESR]
	st.w sp[SF_SR],r11

	//Restore PC
	ld.w r11,r12[TCB.SAVEREG.PC]
	st.w sp[SF_PC],r11

	//Restore registers
	sub r12,-TCB.SAVEREG.LR
	ldmts r12,r0-lr

	rets

//contextRestore:
//	//Fetch the current executing task
//	lda.w r12,EXECTASK
//	ld.w r12,r12[0x0]

//	//Restore USP
//	ld.w r0,r12[TCB.SAVEUSP]
//	st.w --sp,r0
//	ldmts sp,sp
//	ld.w r0,sp++				//Dummy read to correct the stack

//	//Restore LR = retaddr
//	ld.w lr,r12[TCB.SAVEREG.LR]

//	//Restore RETADDR
//	ld.w r0,r12[TCB.RETADDR]
//	st.w sp[SF_PC],r0

//	//Restore registers
//	//~ mov r0,TCB.SAVEREG.R1
//	//~ add r0,r12
//	sub r12,-TCB.SAVEREG.R1
//	ldmts r0,r1-r11

	//Restore SR
//	ld.w r0,r12[TCB.SAVESR]
//	st.w sp[SF_SR],r0			//Put SR on the stack for later fetch

	//Restore work registers
//	//~ ld.w r0,r12[TCB.SAVER0]
//	//~ ld.w r1,r12[TCB.SAVER1]
//	ld.w r0,r12[TCB.SAVEREG.R0]
//	ld.w r12,r12[TCB.SAVEREG.R12]

//	//We are done, exit from supervisor mode
//	rets										//TODO: rets will take care of the interrupEnable stuff!!!!

/*********************************************************
* ROSA_, the users interface to the kernel
*
* Comment:
* Below are the user callable functions that perform
* kernel function calls.
* These functions are called from the kernel API from
* user mode and transfer control to supervisor mode.
*
*********************************************************/
//Note most kernel functions are located in other source files.
/*********************************************************
* ROSA_yield
*
* Comment:
* 	scall helper functions, called from user mode and
* 	transfer control to supervisor mode
* 	They are used for context switching.
*
*********************************************************/
ROSA_yield:
	pushm lr
	lda.w lr,_yield
	//Enter supervisor mode
	scall
	//~ popm pc
	//If interruptDisable is done earlier, remember to manually turn it on here
_yield:
	rcall interruptDisable
	rcall contextSave
	rcall scheduler
	rcall contextRestore
	//Execution will not return to here!

/*********************************************************
* ROSA_taskDestroy
*
* Comment:
* Remove the currently running task from the ROSA system.
* After the task is removed, rescheduling is done to
* continue execution of the remaining tasks in the ROSA
* system.
*********************************************************/
ROSA_taskDestroy:
	rcall interruptDisableIf
	lda.w lr,_ROSA_taskDestroy
	//Enter supervisor mode
	scall
_ROSA_taskDestroy:
	//call interruptDisable
	//Remove the current task
	rcall _taskDestroy
	//Reschedule.
	rcall scheduler
	rcall contextRestore	//Will enable interrupt!!! TODO: Warning
	//Execution will not return to here!

//Return the SSP in r12	TODO: remove
_SSPGet:
	pushm lr
	lda.w lr,_scallSSPGet
	scall
	popm pc
_scallSSPGet:
	mov r12,sp
	rets
//~ //~
//Get the address of the USP TODO: REMOVE
//Return in r12
_USPGet:
	pushm lr
	lda.w lr,_scallUSPGet
	scall
	popm pc
_scallUSPGet:
	mov r12,sp
	st.w --sp,r12
	stmts sp,sp
	ld.w r12,sp++                  //USP in r12
	rets
