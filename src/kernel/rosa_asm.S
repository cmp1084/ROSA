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
 * rosa_asm.S
 *
 *  Created on: Sep 6, 2010
 *      Author: marcus
 */

#include <avr32/io.h>
#include "kernel/rosa_off.i"
#include "rosa_config.h"

	.section .data

	//Kernel functions
	.global ROSA_init_lowlevel
	.global ROSA_start_lowlevel
	.global ROSA_contextInit
	.global ROSA_contextSave
	.global ROSA_contextRestore
	.global ROSA_contextSaveFromISR
	.global ROSA_contextRestoreFromISR

	//Other functions
	.global _ROSA_readSR

	//Mask value for SR register
	.equ SRMASK1,0xfe3e
	.equ SRMASK2,0x001c

	.section .text, "ax", @progbits
/***********************************************************
 * _ROSA_INIT
 *
 * Comment:
 * Set the pointer to the list of TCB's to NULL.
 *
 * C prototype:
 * extern void _ROSA_init(void);
 **********************************************************/
_ROSA_init_lowlevel:
	/*
	 * Additional low level initializations
	 */
	mov pc,lr


/***********************************************************
 * _ROSA_tcbCreate_lowlevel
 *
 * Comment:
 * Initialize the TCB with given contents, this routine
 * is used instead of the TASKDEF makro when the user
 * writes his application tasks in C.
 *
 * C prototype:
 * extern void _ROSA_tcbCreate_lowlevel(tcb * tcbTask,
 *                       char task_name[4],
 *                       void *task_function,
 *                       int *task_stack,
 *                       int task_stack_size);
 **********************************************************/
_ROSA_tcbCreate_lowlevel:
	/*
	 * Additional low level TCB creation
	 */
	mov pc,lr


/***********************************************************
 * _ROSA_tcbInstall_lowlevel
 * Install a task in the static TCB chain
 *
 * Input:
 * r1 - Address to the tasks TCB
 *
 * Comment:
 * Initialize all tasks SP. For each task put the startaddress
 * of the dataarea (from TCB-dataarea) in USP and then store it
 * in the register savearea of the TCB.
 * Note: This is a circular list.
 *
 * C prototype:
 * extern void _ROSA_tcbInstall_lowlevel(tcb * task);
 *
 **********************************************************/
_ROSA_tcbInstall_lowlevel:
	/*
	 * Additional low level tcb installation
	 */
	mov pc,lr


/*********************************************************
 * _ROSA_kernel
 *
 * Comment:
 * _ROSA_kernel execute in supervisor mode
 *
 * C prototype:
 * None
 ********************************************************/
_ROSA_kernel:
	mov pc,lr


/*********************************************************
* _ROSA_SCALL_START kernel code
*
* Comment:
* Start the kernel.
*
*********************************************************/
	//. sc  e c t i o n .data
.equ STACKFRAMESTART_SR,0x00

	//.section .text, "ax", @progbits
_ROSA_scall_start:
	pushm r12
	//Fetch the executing task
	lda.w r12,EXECTASK
	ld.w r12,r12[0x0]

	//Set up start USP
	ld.w r0,r12[TCB.SAVEUSP]
	st.w --sp,r0
	ldmts sp,sp
	ld.w r0,sp++

	//Load start registers
	// - Not necessary/interesting at this point

	//Set up start SR
	ld.w r0,r12[TCB.SAVESR]
	andh r0,SRMASK1
	orh r0,SRMASK2
	st.w sp[STACKFRAMESTART_SR],r0

	//Load start r0, used as work register
	ld.w r0,r12[TCB.SAVER0]

	//Load start lr, will jump here after rets
	ld.w lr,r12[TCB.STADDR]
	popm r12
	rets


/*********************************************************
 * _ROSA_scall_contextInit
 *
 * Comment:
 * The initial save of a tasks context to the tcb
 * from supervisor mode
 *
 * C prototype:
 * None
 ********************************************************/
_ROSA_scall_contextInit:
	//Initialize working regs
	mov r0,0x0
	st.w r12[TCB.SAVER0],r0
	st.w r12[TCB.SAVER1],r1

	//Initialize regs
	// - Not interesting to do at this point.
	// - Leave most registers as they are.

	//Initialize lr in the savereg area
	ld.w r0,r12[TCB.STADDR]
	st.w r12[TCB.SAVEREG.LR],r0
	//~ st.w r12[TCB.SAVEREG.R12],r0	//TODO:

	rets


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
* 	ROSA_Yield() or ROSA_yieldFromISR().
*
*********************************************************/
	//~ .section .data TODO:

//Stack frame, supervisor stack:
//------------------------------
//SR
//PC

//Stack frame, user stack:
//------------------------
//LR_ROSA_yield from ROSA_contextSave
//LR_task from ROSA_yield
//R7_task from ROSA_yield

//SF = Stack Frame
.equ SF_OFFSET,0x00
.equ SF_SR,0x00 + SF_OFFSET
.equ SF_PC,0x04 + SF_OFFSET
.equ SF_LR_RS,0x00 + SF_OFFSET
.equ SF_LR_TASK,0x04 + SF_OFFSET
.equ SF_R7_TASK,0x08 + SF_OFFSET

	//~ .section .text, "ax", @progbits TODO:
/*********************************************************
* _ROSA_scall_contextSave
*
* Comment:
* Task switch routine. This routine save task context.
* Context of the TCB referenced by EXECTASK will be saved.
*
*********************************************************/
_ROSA_scall_contextSave:
	pushm r12,lr
	call ROSA_interruptDisable		//Disable interrupts
	//~ call ROSA_timerStop				//Stop timer		//TODO:
	popm r12,lr

	//We want to save R12 on the stack for a while
	st.w --sp,r12

	//Fetch the current executing task
	lda.w r12,EXECTASK
	ld.w r12,r12[0x0]

	//Save work registers
	st.w r12[TCB.SAVER0],r0
	st.w r12[TCB.SAVER1],r1
	ld.w r0,sp++
	st.w r12[TCB.SAVEREG.R12],r0

	//Get the address of the USP
	mov r0,sp
	st.w --sp,r0
	stmts sp,sp
	ld.w r1,sp++

	//Save SR
	ld.w r0,sp[SF_SR]
	st.w r12[TCB.SAVESR],r0

	//Save registers
	mov r0,TCB.SAVEREG.R11
	add r0,r12
	stmts r0,r0-r11

	//Save r7_task (done automaticlly, but the SaveFromIRQ need it)
	ld.w r0,r1[SF_R7_TASK]			//Get from user stack
	st.w r12[TCB.SAVEREG.R7],r0

	//Save RETADDR
	ld.w r0,r1[SF_LR_TASK]			//Get from user stack, return to task, not to yield
	st.w r12[TCB.RETADDR],r0		//Correction of stack is done later

	//Save LR_task
	st.w r12[TCB.SAVEREG.LR],r0

	//Correct the stack for (ROSA_yield() + contextSave() calls) = 0x03*0x04=0x0c, since we dont want to return to ROSA_yield(), but to the executing task.
	sub r1,-0x0c

	//Save USP
	st.w r12[TCB.SAVEUSP],r1

	//Set USP to SSP until _contextRestore	//TODO:
	mov r0,sp
	st.w --sp,r0
	stmts sp,sp
	ld.w r0,sp++

	//Restore work regs (unnecessary at this point) //TODO:
	//~ ld.w r0,r12[TCB.SAVER0]
	//~ ld.w r0,r12[TCB.SAVER1]

	rets

/*********************************************************
* _ROSA_scall_contextRestore
*
* Comment:
* Taskswitch routine. This routine restore task context.
* Context of the TCB referenced by EXECTASK will be restored.
*
*********************************************************/
_ROSA_scall_contextRestore:
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

	//Restore r7
	ld.w r7,r12[TCB.SAVEREG.R7]

	//Restore SR
	ld.w r0,r12[TCB.SAVESR]
	andh r0,SRMASK1
	orh r0,SRMASK2
	st.w sp[SF_SR],r0

	//Restore work registers
	ld.w r0,r12[TCB.SAVER0]
	ld.w r1,r12[TCB.SAVER1]
	ld.w r12,r12[TCB.SAVEREG.R12]

	//Clear any timer interrupt that have occured and
	//reset the timer counter
	pushm lr
	call ROSA_timerClearInterrupt
	call ROSA_timerReset

	//Enable interrupts
	call ROSA_interruptEnable
	popm lr
	rets


/*********************************************************
* ROSA_contextSaveFromISR
*
* Comment:
* Taskswitch routine. This routine save task context
* from an interrupt.
* Context of the TCB referenced by EXECTASK will be saved.
*
*********************************************************/
//SFINT0 = Stack Frame for interrupt 0
.equ SFINT0OFFSET,0x06*0x04
.equ SFINT0_R8, 0x07*0x04+SFINT0OFFSET
.equ SFINT0_R9, 0x06*0x04+SFINT0OFFSET
.equ SFINT0_R10,0x05*0x04+SFINT0OFFSET
.equ SFINT0_R11,0x04*0x04+SFINT0OFFSET
.equ SFINT0_R12,0x03*0x04+SFINT0OFFSET
.equ SFINT0_LR, 0x02*0x04+SFINT0OFFSET
.equ SFINT0_PC, 0x01*0x04+SFINT0OFFSET
.equ SFINT0_SR, 0x00*0x04+SFINT0OFFSET
.equ SFINT0_R7_TASK,-0x01*0x04+SFINT0OFFSET

ROSA_contextSaveFromISR:
	//Fetch the current executing task
	lda.w r12,EXECTASK
	ld.w r12,r12[0x0]

	//Save working registers
	st.w r12[TCB.SAVER0],r0
	st.w r12[TCB.SAVER1],r1
	ld.w r0,sp[SFINT0_R12]
	st.w r12[TCB.SAVEREG.R12],r0

	//Save SR
	//Mask out mode bits to allow interrupts again
	ld.w r0,sp[SFINT0_SR]
	andh r0,SRMASK1
	orh r0,SRMASK2
	st.w r12[TCB.SAVESR],r0

	//Read register from the stack
	ld.w r8,sp[SFINT0_R8]
	ld.w r9,sp[SFINT0_R9]
	ld.w r10,sp[SFINT0_R10]
	ld.w r11,sp[SFINT0_R11]

	//Save registers
	mov r0,TCB.SAVEREG.R11
	add r0,r12
	stmts r0,r0-r11

	//Save r7_task
	ld.w r0,sp[SFINT0_R7_TASK]
	st.w r12[TCB.SAVEREG.R7],r0

	//Save RETADDR
	ld.w r0,sp[SFINT0_PC]
	st.w r12[TCB.RETADDR],r0

	//Save LR
	ld.w r0,sp[SFINT0_LR]
	st.w r12[TCB.SAVEREG.LR],r0

	//Save USP
	mov r0,sp
	st.w --sp,r0
	stmts sp,sp
	ld.w r0,sp++
	st.w r12[TCB.SAVEUSP],r0
	mov pc,lr

/*********************************************************
* _ROSA_contextRestoreFromISR
*
* Comment:
* Taskswitch routine. This routine restore task context
* from an interrupt.
* Context of the TCB referenced by EXECTASK will be restored.
*
*********************************************************/
ROSA_contextRestoreFromISR:
	//Fetch the current executing task
	lda.w r12,EXECTASK
	ld.w r12,r12[0x0]

	//Restore USP
	ld.w r0,r12[TCB.SAVEUSP]
	st.w --sp,r0
	ldmts sp,sp
	ld.w r0,sp++							//Dummy read to correct sp

	//Restore LR
	ld.w r0,r12[TCB.SAVEREG.LR]
	st.w sp[SFINT0_LR],r0					//Restore lr on the stack

	//Restore RETADDR
	ld.w r0,r12[TCB.RETADDR]
	st.w sp[SFINT0_PC],r0					//Restore return address on stack

	//Restore registers
	mov r0,TCB.SAVEREG.R11
	add r0,r12
	ldmts r0,r0-r11							//r7_irq destroyed here!

	//Restore SR
	ld.w r0,r12[TCB.SAVESR]
	andh r0,SRMASK1
	orh r0,SRMASK2
	st.w sp[SFINT0_SR],r0					//Put the new SR on the stack

	//Restore of R8-R12
	ld.w r0,r12[TCB.SAVEREG.R12]
	st.w sp[SFINT0_R12],r0
	st.w sp[SFINT0_R11],r11
	st.w sp[SFINT0_R10],r10
	st.w sp[SFINT0_R9],r9
	st.w sp[SFINT0_R8],r8

	ld.w r0,r12[TCB.SAVEREG.R7]				//This is the r7_user
	st.w sp[SFINT0_R7_TASK],r0				//Save r7_user to the stack, in order to not have the r7_user destroyed.

	//Restore work registers
	ld.w r0,r12[TCB.SAVER0]
	ld.w r1,r12[TCB.SAVER1]

	//Restart the timer 0
	pushm lr
	call ROSA_timerStart
	popm lr
	mov pc,lr


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
* ROSA_init_lowlevel
* 	and
* ROSA_start_lowlevel
*
* Comment:
* 	scall helper functions, called from user mode and
* 	transfer control to supervisor mode
*
*********************************************************/
ROSA_init_lowlevel:
	pushm lr
	lda.w lr,_ROSA_init_lowlevel
	scall
	ld.w pc,sp++

ROSA_start_lowlevel:
	lda.w lr,_ROSA_scall_start
	scall
	mov pc,lr

/*********************************************************
* ROSA_contextInit
* 	and
* ROSA_contextSave
* 	and
* ROSA_contextRestore
*
* Comment:
* 	scall helper functions, called from user mode and
* 	transfer control to supervisor mode
* 	They are used for context switching.
*
*********************************************************/
ROSA_contextSave:
	pushm lr
	lda.w lr,_ROSA_scall_contextSave
	scall
	ld.w pc,sp++

ROSA_contextRestore:
	pushm lr
	lda.w lr,_ROSA_scall_contextRestore
	scall
	//Execution will not return to here!

ROSA_contextInit:
	st.w --sp,lr
	lda.w lr,_ROSA_scall_contextInit
	scall
	ld.w pc,sp++
