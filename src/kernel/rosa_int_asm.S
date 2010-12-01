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
 * rosa_int_asm.S
 *
 *  Created on: Oct 1, 2010
 *      Author: marcus
 */

#include <avr32/io.h>
#include "kernel/rosa_off.i"

	//General constants
	.equ TRUE,0x01
	.equ FALSE,0x00

	//Global functions
	.global interruptDisable
	.global interruptEnable
	.global isInterruptEnabled
	.global interruptInit
	.global contextSaveFromISR
	.global contextRestoreFromISR

	//Interrupt constants
	.equ INTLEVEL0,0
	.equ INTLEVEL_OFFSET,30
	.equ GRP14,0x0e*0x04					//Grp 14 for TC
	.equ LINE0,0x01							//Line 0 for TC0

	//Interrupt vectors
	//Put function code at an aligned address in the .exception section, make it executable.
	.section  .exception, "ax", @progbits
	.balign 0x200
	.global _evba
	.type _evba, @function
_evba:
  .org  0x000			//TODO: clean up
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
	//~ rjmp privilegedCalls					//Check for legal priviledged calls.

	.org  0x100
_handle_Supervisor_Call:
	mov pc,lr								//ROSA Kernel function calls pass through here.

/***********************************************************
 * ROSA timer interrupt handling, lowlevel
 *
 * Comment:
 * 	Send the interrupt to the proper ISR handler,
 *  in this case: timerISR().
 *
 **********************************************************/
	//TC0 interrupt
_int0:
	mov r12,lo(AVR32_INTC_ADDRESS)
	orh r12,hi(AVR32_INTC_ADDRESS)
	ld.w r12,r12[AVR32_INTC_IRR + GRP14]	//TC irq grp is at GRP14
	andl r12,LINE0							//TC0 irq is at line 0
	cp.w r12,LINE0							//Is line 0 sending interrupt signal?
	brne _int0_exit							//No, exit
	lda.w r12,handler_timerISR						//Pointer to the timer ISR
	mov pc,r12								//Yes, go to the ISR.
_int0_exit:
	rete

/***********************************************************
 * Privileged Calls
 *
 * Comment:
 * 	Handle privileged function calls.
 *  Look for the legal priviledged calls,
 *  Stop execution if this was an illegal priviledged call.
 *
 **********************************************************/
.equ PRIV_STACKOFFSET,0x08					//Due to r11, r12 pushed to stack. 0x04 * 2 = 0x08
.equ USERSR,0x00+PRIV_STACKOFFSET
.equ USERPC,0x04+PRIV_STACKOFFSET
.equ USERR12,0x00

//TODO: make a jumptable out of this, if it is ever worth it
//~ privilegedCallsJumptable:
	//~ dc.w interruptDisable, priv_interruptdisable
	//~ dc.w interruptEnable, priv_interruptenable
	//~ dc.w isInterruptEnabled, priv_isinterruptenable
	//~ dc.w -1,-1

privilegedCalls:
//Stack frame at this point in time:
//------------------------------------
//USERSR - user SR at the offending instant
//USERPC - user PC which contain the offending instruction

	//Investigate if this is a legal disable/enable interrupt requests
	pushm r11,r12
	ld.w r11,sp[USERPC]
	mov r12,r11
	sub r12,-0x02							//Modify to get return address
	st.w sp[USERPC],r12						//Put return address on the stack

	//Look for interruptDisable
priv_interruptdisable:
	lda.w r12,interruptDisable
	cp.w r11,r12
	brne priv_interruptenable
	ld.w r12,sp[USERSR]
	sbr r12,AVR32_SR_GM						//Ok, disable interrupts
	st.w sp[USERSR],r12
	popm r11,r12
	rete

	//Look for interruptEnable
priv_interruptenable:
	lda.w r12,interruptEnable
	cp.w r11,r12
	brne priv_isinterruptenabled
	ld.w r12,sp[USERSR]
	cbr r12,AVR32_SR_GM						//Ok, enable interrupts
	st.w sp[USERSR],r12
	popm r11,r12
	rete

	//Look for isInterruptEnable
priv_isinterruptenabled:
	lda.w r12,isInterruptEnabled
	cp.w r11,r12
	brne privexception
	ld.w r12,sp[USERSR]						//Ok, read interrupt flag
	st.w sp[USERR12],r12
	popm r11,r12
	rete

privexception:								//Not ok.
	rjmp $									//End here as we received an illegal privileged call.


/***********************************************************
 * interruptInit
 *
 * Comment:
 * 	Interrupt setup routines
 *
 * C prototypes:
 * 	extern void interruptInit(void);
 **********************************************************/
interruptInit:
	pushm r11,r12
	//Get autovector offset to _int0
	lda.w r11,_int0
	lda.w r12,_evba
	sub r11,r12

	//Calculate int level
	mov r12,INTLEVEL0
	or r11,r11,r12 << INTLEVEL_OFFSET

	//Store the _int0 level and autovector offset to the interrupt priority register
	mov r12,lo(AVR32_INTC_ADDRESS+GRP14)
	orh r12,hi(AVR32_INTC_ADDRESS+GRP14)
	st.w r12,r11

	popm r11,r12
	mov pc,lr


/*********************************************************
 * interruptDisable
 * interruptEnable
 * isInterruptEnabled
 * contextSaveFromISR
 * contextRestoreFromISR
 *
 * Comment:
 * 	Helper functions, called from user mode and
 * 	transfer control to supervisor mode
 *  They are used for kernel controll and context switching.
 *
 *********************************************************/
/**********************************************************
 * interruptDisable
 *
 * Comment:
 * 	Disable global interrupts.
 *
 * C prototype:
 * 	void interruptDisable(void);
 *
 *********************************************************/
interruptDisable:
	ssrf AVR32_SR_GM
	mov pc,lr


/**********************************************************
 * interruptEnable
 *
 * Comment:
 * 	Enable global interrupts.
 *
 * C prototype:
 * 	void interruptEnable(void);
 *
 *********************************************************/
interruptEnable:
	csrf AVR32_SR_GM
	mov pc,lr


/**********************************************************
 * isInterruptEnabled
 *
 * Comment:
 * 	Check if global interrupts are enabled.
 *
 * Returns:
 * TRUE or FALSE
 *
 * C prototype:
 * 	int isInterruptEnabled(void);
 *
 *********************************************************/
isInterruptEnabled:
	mfsr r12,0
	lsr r12,AVR32_SR_GM
	andl r12,TRUE
	eorl r12,TRUE
	mov pc,lr


/*********************************************************
* contextSaveFromISR
*
* Comment:
* Context switch routine. This routine save task context
* from an interrupt.
* Context of the TCB referenced by EXECTASK will be saved.
*
*********************************************************/
//NOTE: There is two versions of contextSaveFromISR and contextRestoreFromISR
//The first one is used when optimization is -O0
//The second one is used when optimization is -Os (and hopefully can be used with -O1, 2, 3 as well)
//This work with gcc --version 4.3.2

//SFINT0 = Supervisor Stack Frame for interrupt 0
#if(O0)

//At this point the supervisor stack look like this:
//+------------------------+
//| # Local variables * 4  |
//|   handler_timerISR     |
//+------------------------+
//| LR   ROSA_yieldFromISR |
//| R7   ROSA_yieldFromISR |
//+------------------------+
//| LR   handler_timerISR  |
//| R7   handler_timerISR  |
//+------------------------|
//| SR   From the          |
//| PC   interrupt event   |
//| LR                     |
//| R12                    |
//| R11                    |
//| R10                    |
//| R9                     |
//| R8                     |
//+------------------------+
//| ...                    |
//+------------------------+
//
//The only interesting thing about the user stack is the USP, which is saved into the TCB.

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

contextSaveFromISR:
	//Fetch the current executing task
	lda.w r12,EXECTASK
	ld.w r12,r12[0x0]

	//Save work registers
	st.w r12[TCB.SAVER0],r0
	ld.w r0,sp[SFINT0_R12]
	st.w r12[TCB.SAVEREG.R12],r0

	//Save SR
	ld.w r0,sp[SFINT0_SR]
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
	ld.w r0,sp[SFINT0_LR]			//This points back to the task
	st.w r12[TCB.SAVEREG.LR],r0

	//Save USP
	mov r0,sp
	st.w --sp,r0
	stmts sp,sp
	ld.w r0,sp++
	st.w r12[TCB.SAVEUSP],r0

	mov pc,lr

/*********************************************************
* contextRestoreFromISR
*
* Comment:
* Context switch routine. This routine restore task context
* from an interrupt.
* Context of the TCB referenced by EXECTASK will be restored.
*
*********************************************************/
contextRestoreFromISR:
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

	//Return back to the ISR handler
	mov pc,lr
//#endif
#else


//This is the second version for optimized compilation
//#if(Os)

//At this point the supervisor stack look like this:
//
//+------------------------+
//| LR   ROSA_yieldFromISR |
//+------------------------+
//| LR   handler_timerISR  |
//+------------------------|
//| SR   From the          |
//| PC   interrupt event   |
//| LR                     |
//| R12                    |
//| R11                    |
//| R10                    |
//| R9                     |
//| R8                     |
//+------------------------+
//| ...                    |
//+------------------------+
//
//The only interesting thing about the user stack is the USP, which is saved into the TCB.

.equ SFINT0OFFSET,0x02*0x04

.equ SFINT0_SR, 0x00*0x04+SFINT0OFFSET
.equ SFINT0_PC, 0x01*0x04+SFINT0OFFSET
.equ SFINT0_LR, 0x02*0x04+SFINT0OFFSET
.equ SFINT0_R12,0x03*0x04+SFINT0OFFSET
.equ SFINT0_R11,0x04*0x04+SFINT0OFFSET
.equ SFINT0_R10,0x05*0x04+SFINT0OFFSET
.equ SFINT0_R9, 0x06*0x04+SFINT0OFFSET
.equ SFINT0_R8, 0x07*0x04+SFINT0OFFSET

contextSaveFromISR:
	//Fetch the current executing task
	lda.w r12,EXECTASK
	ld.w r12,r12[0x0]

	//Save work registers
	st.w r12[TCB.SAVER0],r0
	ld.w r0,sp[SFINT0_R12]
	st.w r12[TCB.SAVEREG.R12],r0

	//Save SR
	ld.w r0,sp[SFINT0_SR]
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
	//Save RETADDR
	ld.w r0,sp[SFINT0_PC]
	st.w r12[TCB.RETADDR],r0

	//Save LR
	ld.w r0,sp[SFINT0_LR]			//This points back to the task
	st.w r12[TCB.SAVEREG.LR],r0

	//Save USP
	mov r0,sp
	st.w --sp,r0
	stmts sp,sp
	ld.w r0,sp++
	st.w r12[TCB.SAVEUSP],r0

	mov pc,lr


contextRestoreFromISR:
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

	//Restore work registers
	ld.w r0,r12[TCB.SAVER0]

	//Return back to the ISR handler
	mov pc,lr
#endif
