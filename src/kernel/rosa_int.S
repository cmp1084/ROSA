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
_evba:
	.org  0x28
_handle_Privilege_Violation:
	rjmp privilegedCalls

	.org  0x100
_handle_Supervisor_Call:
	mov pc,lr

/***********************************************************
 * ROSA_timer interrupt handling, lowlevel
 *
 * Comment:
 * 	Send the interrupt to the proper ISR handler,
 * 	ROSA_timerISR
 **********************************************************/
_int0:
	//TC0 interrupt
	mov r12,lo(AVR32_INTC_ADDRESS)
	orh r12,hi(AVR32_INTC_ADDRESS)
	ld.w r12,r12[AVR32_INTC_IRR + GRP14]	//TC irq grp is at GRP14
	andl r12,LINE0							//TC0 irq is at line 0 = 1
	cp.w r12,LINE0
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
	brne priv_readSR
	ld.w r12,sp[USERSR]
	cbr r12,AVR32_SR_GM
	st.w sp[USERSR],r12
	popm r11,r12
	rete

	//Look for readSR
priv_readSR:
	lda.w r12,(_ROSA_readSR)
	cp.w r11,r12
	brne privexception
	mfsr r12,0x0
	st.w sp[0x0],r12						//Return result on stack
	popm r11,r12
	rete

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
	andl r11,0xfffe
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
	pushm lr
	call _ROSA_readSR
	lsr r12,AVR32_SR_GM
	andl r12,TRUE
	eorl r12,TRUE
	popm lr
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
_ROSA_readSR:
	mfsr r12,0
	mov pc,lr
