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
 *  Created on: Oct 1, 2010
 *      Author: marcus
 */

#include <avr32/io.h>
#include "kernel/rosa_off.i"
#include "rosa_config.h"

	//General constants
	.equ TRUE,0x01
	.equ FALSE,0x00

	.global ROSA_interruptDisable
	.global ROSA_interruptEnable
	.global ROSA_isInterruptEnabled
	.global ROSA_interruptInit
	.global ROSA_contextSaveFromISR
	.global ROSA_contextRestoreFromISR

	//Other functions
	.global _ROSA_readSR

	//Interrupt constants
	.equ INTLEVEL0,0
	.equ INTLEVEL_OFFSET,30
	.equ GRP14,0x0e*0x04			//Grp 14 for TC
	.equ LINE0,0x01					//Line 0 for TC0

	//Interrupt vectors
	.section  .exception, "ax", @progbits
	.balign 0x200
	.global _evba
	.type _evba, @function
_evba:    .org  0x000								//TODO: clean up!
        // Unrecoverable Exception.
_handle_Unrecoverable_Exception:
        rjmp $

        .org  0x004
        // TLB Multiple Hit.
_handle_TLB_Multiple_Hit:
        rjmp $

        .org  0x008
        // Bus Error Data Fetch.
_handle_Bus_Error_Data_Fetch:
        rjmp $

        .org  0x00C
         // Bus Error Instruction Fetch.
_handle_Bus_Error_Instruction_Fetch:
        rjmp $

        .org  0x010
        // NMI.
_handle_NMI:
        rjmp $

        .org  0x014
        // Instruction Address.
_handle_Instruction_Address:
        rjmp $

        .org  0x018
        // ITLB Protection.
_handle_ITLB_Protection:
        rjmp $

        .org  0x01C
        // Breakpoint.
_handle_Breakpoint:
        rjmp $

        .org  0x020
        // Illegal Opcode.
_handle_Illegal_Opcode:
        rjmp $

        .org  0x024
        // Unimplemented Instruction.
_handle_Unimplemented_Instruction:
        rjmp $

        .org  0x028
        // Privilege Violation.
_handle_Privilege_Violation:
        rjmp privilegedCalls

        .org  0x02C
        // Floating-Point: UNUSED IN AVR32UC and AVR32AP.
_handle_Floating_Point:
        rjmp $

        .org  0x030
        // Coprocessor Absent: UNUSED IN AVR32UC.
_handle_Coprocessor_Absent:
        rjmp $

        .org  0x034
        // Data Address (Read).
_handle_Data_Address_Read:
        rjmp $

        .org  0x038
        // Data Address (Write).
_handle_Data_Address_Write:
        rjmp $

        .org  0x03C
        // DTLB Protection (Read).
_handle_DTLB_Protection_Read:
        rjmp $

        .org  0x040
        // DTLB Protection (Write).
_handle_DTLB_Protection_Write:
        rjmp $

        .org  0x044
        // DTLB Modified: UNUSED IN AVR32UC.
_handle_DTLB_Modified:
        rjmp $

        .org  0x050
        // ITLB Miss.
_handle_ITLB_Miss:
        rjmp $

        .org  0x060
        // DTLB Miss (Read).
_handle_DTLB_Miss_Read:
        rjmp $

        .org  0x070
        // DTLB Miss (Write).
_handle_DTLB_Miss_Write:
        rjmp $

	//~ .org  0x28
//~ _handle_Privilege_Violation:
	//~ rjmp privilegedCalls

	.org  0x100
_handle_Supervisor_Call:
	mov pc,lr

/***********************************************************
 * ROSA_timer interrupt handling, lowlevel
 *
 * Comment:
 * 	Send the interrupt to the proper ISR handler, ROSA_timerISR.
 * 
 **********************************************************/
_int0:
	//TC0 interrupt
	mov r12,lo(AVR32_INTC_ADDRESS)
	orh r12,hi(AVR32_INTC_ADDRESS)
	ld.w r12,r12[AVR32_INTC_IRR + GRP14]	//TC irq grp is at GRP14
	andl r12,LINE0							//TC0 irq is at line 0 = 1
	cp.w r12,LINE0							//Is line 0 sending interrupt signal?
	brne _int0_u
	lda.w r12,ROSA_timerISR					//The pointer to the ISR routine
	mov pc,r12
_int0_u:
	rete

	.section .data
.equ PRIV_STACKOFFSET,0x08					//Due to r11, r12 pushed to stack
.equ USERSR,0x00+PRIV_STACKOFFSET
.equ USERPC,0x04+PRIV_STACKOFFSET

.section .text, "ax", @progbits

/***********************************************************
 * Privileged Calls
 *
 * Comment:
 * 	Handle privileged function calls.
 *
 **********************************************************/
privilegedCalls:
//Stack frame:
//-------------
//SR
//PC offender

	//Check for legal disable/enable interrupt requests
	pushm r11,r12
	ld.w r11,sp[USERPC]
	mov r12,r11
	sub r12,-0x02							//Modify to get return address
	st.w sp[USERPC],r12						//Put return address on the stack

	//Look for interruptDisable
priv_interruptdisable:
	lda.w r12,(ROSA_interruptDisable)
	cp.w r11,r12
	brne priv_interruptenable
	ld.w r12,sp[USERSR]
	sbr r12,AVR32_SR_GM
	st.w sp[USERSR],r12
	popm r11,r12
	rete

	//Look for interruptEnable
priv_interruptenable:
	lda.w r12,(ROSA_interruptEnable)
	cp.w r11,r12
	brne privexception
	ld.w r12,sp[USERSR]
	cbr r12,AVR32_SR_GM
	st.w sp[USERSR],r12
	popm r11,r12
	rete

	//Look for readSR
//~ priv_readSR:
	//~ lda.w r12,(_ROSA_readSR)
	//~ cp.w r11,r12
	//~ brne privexception
	//~ mfsr r12,0x0
	//~ st.w sp[0x0],r12						//Return result on stack
	//~ popm r11,r12
	//~ rete

privexception:
	rjmp $


/***********************************************************
 * ROSA_interruptInit
 *
 * Comment:
 * 	Interrupt setup routines
 *
 * C prototypes:
 * 	extern void ROSA_interruptInit(void);
 **********************************************************/
ROSA_interruptInit:
	pushm r11,r12,lr
	//Get offset to _int0
	lda.w r11,_int0
	lda.w r12,_evba
	sub r11,r12
	//Calculate int level
	andl r11,0xfffe			//TODO: Unnecessary insn
	mov r12,INTLEVEL0
	or r11,r11,r12 << INTLEVEL_OFFSET

	//Store the _int0 level and autovector offset to INTC IPR
	mov r12,lo(AVR32_INTC_ADDRESS)
	orh r12,hi(AVR32_INTC_ADDRESS)
	mov r10,AVR32_INTC_IPR
	add r12,r10
	mov r10,AVR32_INTC_NUM_INT_GRPS
ROSA_initint_1:
	st.w r12++,r11

	sub r10,0x01
	cp.w r10,0x00
	brpl ROSA_initint_1
	popm r11,r12,lr
	mov pc,lr

_unhandled_interrupt:
	rjmp $


/*********************************************************
 * ROSA_interruptDisable
 * ROSA_interruptEnable
 * ROSA_isInterruptEnabled
 * _ROSA_readSR
 *
 * Comment:
 * 	scall helper functions, called from user mode and
 * 	transfer control to supervisor mode
 * 	They are used for context switching.
 *
 *********************************************************/

/**********************************************************
 * ROSA_interruptDisable
 *
 * Comment:
 * 	Disable global interrupts.
 *
 * C prototype:
 * 	void ROSA_interruptDisable(void);
 *
 *********************************************************/
ROSA_interruptDisable:
	ssrf AVR32_SR_GM
	mov pc,lr


/**********************************************************
 * ROSA_interruptEnable
 *
 * Comment:
 * 	Enable global interrupts.
 *
 * C prototype:
 * 	void ROSA_interruptEnable(void);
 *
 *********************************************************/
ROSA_interruptEnable:
	csrf AVR32_SR_GM
	mov pc,lr


/**********************************************************
 * ROSA_isInterruptEnabled
 *
 * Comment:
 * 	Check if global interrupts are enabled.
 *
 * Returns:
 * TRUE or FALSE
 *
 * C prototype:
 * 	int ROSA_isInterruptEnabled(void);
 *
 *********************************************************/
ROSA_isInterruptEnabled:
	//~ pushm lr
	//~ call _ROSA_readSR
	mfsr r12,0
	lsr r12,AVR32_SR_GM
	andl r12,TRUE
	eorl r12,TRUE
	//~ popm lr
	mov pc,lr

/**********************************************************
 * _ROSA_readSR
 *
 * Comment:
 * 	Read the status register.
 *
 * Returns:
 * 	The value of the status register in r12
 *
 * C prototype:
 * 	int _ROSA_readSR(void);
 *
 *********************************************************/
//~ _ROSA_readSR:
	//~ mfsr r12,0
	//~ mov pc,lr

/*********************************************************
* ROSA_contextSaveFromISR
*
* Comment:
* Taskswitch routine. This routine save task context
* from an interrupt.
* Context of the TCB referenced by EXECTASK will be saved.
*
*********************************************************/
//SFINT0 = Supervisor Stack Frame for interrupt 0
.equ SFINT0OFFSET,0x06*0x04

.equ SFINT0_R7_TASK,-0x01*0x04+SFINT0OFFSET
.equ SFINT0_SR, 0x00*0x04+SFINT0OFFSET
.equ SFINT0_PC, 0x01*0x04+SFINT0OFFSET
.equ SFINT0_LR, 0x02*0x04+SFINT0OFFSET
.equ SFINT0_R12,0x03*0x04+SFINT0OFFSET
.equ SFINT0_R11,0x04*0x04+SFINT0OFFSET
.equ SFINT0_R10,0x05*0x04+SFINT0OFFSET
.equ SFINT0_R9, 0x06*0x04+SFINT0OFFSET
.equ SFINT0_R8, 0x07*0x04+SFINT0OFFSET

ROSA_contextSaveFromISR:
	//Fetch the current executing task
	lda.w r12,EXECTASK
	ld.w r12,r12[0x0]

	//Save working registers
	st.w r12[TCB.SAVER0],r0
	//st.w r12[TCB.SAVER1],r1
	ld.w r0,sp[SFINT0_R12]
	st.w r12[TCB.SAVEREG.R12],r0

	//Save SR
	//Mask out mode bits to allow interrupts again
	ld.w r0,sp[SFINT0_SR]
	//andh r0,SRMASK1
	//orh r0,SRMASK2
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
	ld.w r0,sp[SFINT0_LR]			//This points back to task1
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
	ld.w r0,sp++							//USP is in r0

	//Restore LR
	ld.w r0,r12[TCB.SAVEREG.LR]
	st.w sp[SFINT0_LR],r0					//Restore lr on the stack

	//Restore RETADDR
	ld.w r0,r12[TCB.RETADDR]	
	st.w sp[SFINT0_PC],r0					//Restore return address on stack

	//Restore registers
	mov r0,TCB.SAVEREG.R11
	add r0,r12
	ldmts r0,r0-r11

	//Restore SR
	ld.w r0,r12[TCB.SAVESR]
	st.w sp[SFINT0_SR],r0					//Put the new SR on the stack

	//Restore of R8-R12
	ld.w r0,r12[TCB.SAVEREG.R12]
	st.w sp[SFINT0_R12],r0
	st.w sp[SFINT0_R11],r11
	st.w sp[SFINT0_R10],r10
	st.w sp[SFINT0_R9],r9
	st.w sp[SFINT0_R8],r8

	//Restore r7
	ld.w r0,r12[TCB.SAVEREG.R7]				//This is the r7_user
	st.w sp[SFINT0_R7_TASK],r0				//Save r7_user to the stack, in order to not have the r7_user destroyed.

	//Restore work registers
	ld.w r0,r12[TCB.SAVER0]
	//ld.w r1,r12[TCB.SAVER1]

	//Restart timer 0, //TODO: Necessary as timer is stopped by the irq event!
	pushm lr
	call ROSA_timerStart
	popm lr
	
	//Return back to the ISR handler
	mov pc,lr
