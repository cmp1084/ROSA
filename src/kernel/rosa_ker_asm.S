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
//~ #include "rosa_config.h"

	//Put the code and data in the executable .text section
	.section .text, "ax", @progbits

	//Global Kernel functions
	.global ROSA_start
	.global ROSA_contextInit
	.global ROSA_contextSwitch

/*********************************************************
* ROSA_start kernel code
*
* Comment:
* Start the kernel.
* Do the initial context switch to the first task in
* the TCBLIST.
*
*********************************************************/
ROSA_start:
	//Put the first task from TCBLIST into EXECTASK
	lda.w r12,TCBLIST
	lda.w r11,EXECTASK
	ld.w r12,r12[0x0]
	st.w r11[0x0],r12

	//Set up start USP
	ld.w r0,r12[TCB.SAVEUSP]
	st.w --sp,r0
	ldmts sp,sp
	ld.w r0,sp++

	//Load start lr, execution will jump here later
	ld.w lr,r12[TCB.STADDR]

	//Set up start SR, enter user mode
	ld.w r0,r12[TCB.SAVESR]
	mtsr 0x0,r0

	//Load start registers
	ld.w r0,r12[TCB.SAVEREG.R0]
	ld.w r1,r12[TCB.SAVEREG.R1]
	ld.w r2,r12[TCB.SAVEREG.R2]
	ld.w r3,r12[TCB.SAVEREG.R3]
	ld.w r4,r12[TCB.SAVEREG.R4]
	ld.w r5,r12[TCB.SAVEREG.R5]
	ld.w r6,r12[TCB.SAVEREG.R6]
	ld.w r7,r12[TCB.SAVEREG.R7]
	ld.w r8,r12[TCB.SAVEREG.R8]
	ld.w r9,r12[TCB.SAVEREG.R9]
	ld.w r10,r12[TCB.SAVEREG.R10]
	ld.w r11,r12[TCB.SAVEREG.R11]
	ld.w r12,r12[TCB.SAVEREG.R12]

	mov pc,lr


/*********************************************************
 * ROSA_contextInit
 *
 * Comment:
 * The initial save of a tasks context to the tcb
 * from supervisor mode.
 *
 ********************************************************/
ROSA_contextInit:
	//Initialize lr in the savereg area
	ld.w r0,r12[TCB.STADDR]
	st.w r12[TCB.SAVEREG.LR],r0

	//Initialize regs to zero
	mov r0,0x0
	st.w r12[TCB.SAVEREG.R0],r0
	st.w r12[TCB.SAVEREG.R1],r0
	st.w r12[TCB.SAVEREG.R2],r0
	st.w r12[TCB.SAVEREG.R3],r0
	st.w r12[TCB.SAVEREG.R4],r0
	st.w r12[TCB.SAVEREG.R5],r0
	st.w r12[TCB.SAVEREG.R6],r0
	st.w r12[TCB.SAVEREG.R7],r0
	st.w r12[TCB.SAVEREG.R8],r0
	st.w r12[TCB.SAVEREG.R9],r0
	st.w r12[TCB.SAVEREG.R10],r0
	st.w r12[TCB.SAVEREG.R11],r0
	st.w r12[TCB.SAVEREG.R12],r0
	mov pc,lr


/*********************************************************
* _ROSA_TASK_SWITCH kernel code, various functions:
* 	_ROSA_scall_contextSave
* 	_ROSA_scall_contextRestore
* 	_ROSA_scall_contextSaveFromISR
* 	_ROSA_scall_contextRestoreFromISR
*
* Comment:
* 	Task switch routines.
* 	These routines are executed after a call to
* 	ROSA_contextSwitch() or ROSA_contextSwitchFromISR().
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

/*********************************************************
* _ROSA_scall_contextSave
*
* Comment:
* Task switch routine. This routine save task context.
* Context of the TCB referenced by EXECTASK will be saved.
*
*********************************************************/
_ROSA_contextSave:
	pushm r12
	//Fetch the current executing task
	lda.w r12,EXECTASK
	ld.w r12,r12[0x0]

	//Save work registers to TCB
	st.w r12[TCB.SAVER0],r0
	st.w r12[TCB.SAVER1],r1
	ld.w r0,sp++					//Use r0 to save r12
	st.w r12[TCB.SAVEREG.R12],r0

	//Save task SR to TCB
	ld.w r0,sp[SF_SR]
	st.w r12[TCB.SAVESR],r0

	//Save task registers r0-r11 to TCB
	mov r0,TCB.SAVEREG.R11
	add r0,r12
	stmts r0,r0-r11

	//Get the address of the USP
	mov r0,sp
	st.w --sp,r0
	stmts sp,sp
	ld.w r1,sp++					//USP in r1

	//Save RETADDR to TCB
	ld.w r0,r1[SF_LR_TASK]			//Get from user stack, return to task, not to contextSwitch
	st.w r12[TCB.RETADDR],r0		//Correction of stack is done later

	//Save LR_task
	st.w r12[TCB.SAVEREG.LR],r0

	//Correct the USP for the ROSA_contextSwitch() call.
	//This is done since we do not want to return to ROSA_contextSwitch(),
	//where the stack frame is currently at, but to the executing task.
	sub r1,-0x04

	//Save USP
	st.w r12[TCB.SAVEUSP],r1

	mov pc,lr

/*********************************************************
* _ROSA_contextRestore
*
* Comment:
* Taskswitch routine. This routine restore task context.
* Context of the TCB referenced by EXECTASK will be restored.
*
*********************************************************/
_ROSA_contextRestore:
	//Fetch the current executing task
	lda.w r12,EXECTASK
	ld.w r12,r12[0x0]

	//Restore USP
	ld.w r1,r12[TCB.SAVEUSP]
	st.w --sp,r1
	ldmts sp,sp
	ld.w r1,sp++

	//Restore LR = retaddr
	ld.w lr,r12[TCB.SAVEREG.LR]

	//Restore RETADDR
	ld.w r0,r12[TCB.RETADDR]
	st.w sp[SF_PC],r0

	//Restore registers
	mov r0,TCB.SAVEREG.R11
	add r0,r12
	ldmts r0,r0-r11

	//Restore r7				//TODO: Baby, please...
	//ld.w r7,r12[TCB.SAVEREG.R7]

	//Restore SR
	ld.w r0,r12[TCB.SAVESR]
	st.w sp[SF_SR],r0			//Put SR on the stack for later fetch

	//Restore work registers
	ld.w r0,r12[TCB.SAVER0]
	ld.w r1,r12[TCB.SAVER1]
	ld.w r12,r12[TCB.SAVEREG.R12]

	//We are done, exit from supervisor mode
	rets

/*********************************************************
* _ROSA_ the users interface to the kernel
*
* Comment:
* Below are the user callable functions that perform
* kernel function calls.
* These functions are called from the kernel API from
* user mode and transfer control to supervisor mode.
*
*********************************************************/

/*********************************************************
* ROSA_contextSwitch
*
* Comment:
* 	scall helper functions, called from user mode and
* 	transfer control to supervisor mode
* 	They are used for context switching.
*
*********************************************************/
ROSA_contextSwitch:
	pushm lr
	lda.w lr,_ROSA_contextSwitch
	//Enter supervisor mode
	scall
_ROSA_contextSwitch:
	call _ROSA_contextSave
	call ROSA_scheduler
	call _ROSA_contextRestore
	//Execution will not return to here!
