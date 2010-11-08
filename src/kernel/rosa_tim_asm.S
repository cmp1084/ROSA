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

#include <avr32/io.h>
#include "rosa_config.h"

.section .data
	//Timer variables
timerRC:
	.word 0
timerPrescale:
	.word 0

	//Timer functions
	.global timerInit
	.global timerStart
	.global timerStop
	.global timerReset
	.global timerRCSet
	.global timerPrescaleSet
	.global timerClearInterrupt

	//Timer variables
	.global timerPrescale
	.global timerRC

.section .text, "ax", @progbits
/***********************************************************
 * timer functions, various
 *
 * Comment:
 * 	Functions to setup and control timer 0
 *
 * C prototypes:
 * 	extern void timerInit(void);
 *  extern void timerStart(void);
 *  extern void timerReset(void);
 *  extern void timerStop(void);
 *  extern void timerClearInterrupt(void);
 *  extern void timerRCSet(void);
 **********************************************************/
//Timer CMR0
.equ WAVE,(1 << AVR32_TC_WAVE_OFFSET)
.equ WAVSEL,(2 << AVR32_TC_WAVSEL_OFFSET) //Up mode with automatic trigger on RC Compare

//Timer SR
.equ CPCS,0x01

/***********************************************************
 * timerInit
 *
 * Comment:
 * 	Setup the TC0 timer.
 *	Actually _starting_ the timer is done by timerStart();
 *
 * C prototypes:
 * 	extern void timerInit(unsigned ms);
 **********************************************************/
timerInit:
	pushm r11,lr
	pushm r12
	//Remove any remaining timer interrupts
	call interruptDisable;
	call timerClearInterrupt

	//Setup timer waveform
	mov r12,lo(AVR32_TC_ADDRESS)
	orh r12,hi(AVR32_TC_ADDRESS)
	mov r11,lo(WAVE | WAVSEL)
	orh r11,hi(WAVE | WAVSEL)
	st.w r12[AVR32_TC_CMR0],r11

	//Enable TC interrupt
	mov r11,CPCS
	lsl r11,AVR32_TC_CPCS_OFFSET
	st.w r12[AVR32_TC_IER0],r11

	//Set the timer period
	popm r12
	call timerPeriodSet

	popm r11,lr
	mov pc,lr


/***********************************************************
 * timerStart and timerReset
 *
 * Comment:
 * 	Start/Reset the TC0 timer
 *	This also reloads RC from timerRC
 *
 * C prototypes:
 *  extern void timerStart(void);
 *  extern void timerReset(void);
 **********************************************************/
timerStart:
timerReset:
	pushm r11,r12
	mov r12,lo(AVR32_TC_ADDRESS)
	orh r12,hi(AVR32_TC_ADDRESS)
	mov r11,((1 << AVR32_TC_CCR0_CLKEN) | (1 << AVR32_TC_CCR0_SWTRG))
	st.w r12[AVR32_TC_CCR0],r11

	//Init RC counter value
	mov r11,lo(timerRC)
	orh r11,hi(timerRC)
	ld.w r11,r11[0x0]
	st.w r12[AVR32_TC_RC0],r11
	popm r11,r12
	mov pc,lr


/***********************************************************
 * timerStop
 *
 * Comment:
 * 	Stop the TC0 timer
 *
 * C prototypes:
 *  extern void timerStop(void);
 **********************************************************/
timerStop:
	pushm r11,r12
	mov r12,lo(AVR32_TC_ADDRESS)
	orh r12,hi(AVR32_TC_ADDRESS)
	mov r11,1 << AVR32_TC_CCR0_CLKDIS
	st.w r12[AVR32_TC_CCR0],r11
	popm r11,r12
	mov pc,lr


/***********************************************************
 * timerRCSet
 *
 * Comment:
 * 	Set the RC value of the timer
 * 	No checking for sane limits is done.
 *	The current value of the RC can be read through
 *	the variable timerRC.
 *
 * C prototypes:
 *  extern void timerRCSet(int);
 **********************************************************/
timerRCSet:
	pushm r11,r12
	lda.w r11,timerRC
	st.w r11[0x0],r12
	mov r12,lo(AVR32_TC_ADDRESS)
	orh r12,hi(AVR32_TC_ADDRESS)
	ld.w r11,r11[0x0]
	st.w r12[AVR32_TC_RC0],r11
	popm r11,r12
	mov pc,lr


/***********************************************************
 * timerPrescaleSet
 *
 * Comment:
 * 	Set the prescale value of the timer
 * 	No checking for sane limits is done.
 *	The current value for the prescale can be read through
 * 	the variable timerPrescale.
 *
 * C prototypes:
 *  extern void timerPrescaleSet(int);
 **********************************************************/
timerPrescaleSet:
	pushm r10,r11,r12
	lda.w r11,timerPrescale
	st.w r11[0x0],r12
	mov r12,lo(AVR32_TC_ADDRESS)
	orh r12,hi(AVR32_TC_ADDRESS)
	ld.w r10,r12[AVR32_TC_CMR0]
	mov r11,~AVR32_TC_TCCLKS_MASK
	and r10,r11
	lda.w r11,timerPrescale
	ld.w r11,r11[0x0]
	or r11,r10
	st.w r12[AVR32_TC_CMR0],r11
	popm r10,r11,r12
	mov pc,lr


/***********************************************************
 * timerClearInterrupt
 *
 * Comment:
 * 	Read TC0.SR in order to clear the interrupt flag
 *
 * C prototypes:
 *  extern void timerClearInterrupt(void);
 **********************************************************/
timerClearInterrupt:
	pushm r12
	mov r12,lo(AVR32_TC_ADDRESS)
	orh r12,hi(AVR32_TC_ADDRESS)
	ld.w r12,r12[AVR32_TC_SR0]
	popm r12
	mov pc,lr
