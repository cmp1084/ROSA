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

	.section .data
	.global potInit
	.global potGetValue

	.equ GPIO,0xffff1000
	.equ ADC,0xffff3c00

	.equ GPIOGPERC,0x08
	.equ GPIOPMR0C,0x18
	.equ GPIOPMR1C,0x28
	.equ PA22,0x16

	.equ ADCCR,0x00
	.equ ADCCH,0x02
	.equ ADCMR,0x04
	.equ ADCCHER,0x10
	.equ ADCCHDR,0x14
	.equ ADCCHSR,0x18
	.equ ADCSR,0x1c
	.equ ADCLCDR,0x20
	.equ ADCCDR1,0x34

	.section .text, "ax", @progbits

/***********************************************************
 * potInit
 *
 * Comment:
 * 	Initialize the potentiometer.
 *
 * C prototypes:
 * 	extern void potInit(void);
 **********************************************************/
potInit:
	pushm r11,r12
	//Set up potentiometer
	mov r12,lo(GPIO)
	orh r12,hi(GPIO)
	mov r11,PA22
	st.w r12[GPIOGPERC],r11
	st.w r12[GPIOPMR0C],r11
	st.w r12[GPIOPMR0C],r11

	//Set mode
	mov r12,lo(ADC)
	orh r12,hi(ADC)
	mov r11,0x01
	st.w r12[ADCCR],r11
	mov r11,0x0f1f
	lsl r11,0x10
	st.w r12[ADCMR],r11

	//Enable potentiometer
	mov r11,ADCCH
	st.w r12[ADCCHER],r11
	popm r11,r12
	mov pc,lr


/***********************************************************
 * potGetValue
 *
 * Comment:
 * 	Get the potentiometer value.
 *
 * C prototypes:
 * 	extern int potGetValue(void);
 **********************************************************/
potGetValue:
	pushm r10,r11
	mov r12,lo(ADC)
	orh r12,hi(ADC)
	mov r11,ADCCH
	st.w r12[ADCCR],r11
a1:	ld.w r10,r12[ADCSR]
	tst r10,r11
	breq a1
	ld.w r12,r12[ADCCDR1]
	popm r10,r11
	mov pc,lr
