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
 * rosa_off.h
 *
 * Log:	Date		Ver		History
 * 		20100728 	1.0		First AVR32 version, Marcus Jansson
 *
 *
 * Tab size: 4
 *
 */

#ifndef _ROSA_OFF_H_
#define _ROSA_OFF_H_
/***********************************************************
 * TCB OFFSETS
 *
 * Comment:
 * 	These are the offsets ROSA low level routines uses to
 * 	reach into the tcb struct.
 *
 **********************************************************/
.offset  0
TCB.NEXTTCB:	.word 0		/* Pointer to next tcb in chain */
TCB.ID:			.word 0		/* Task identifier, 4 ASCII characters */
TCB.STADDR:		.word 0		/* Startaddress of taskcode */
TCB.DATAAREA:	.word 0		/* Startaddress of dataarea (stack) */
TCB.DATASIZE:	.word 0		/* Size of dataarea, used in initialization */
TCB.SAVEUSP:	.word 0 	/* User stackpointer. */
TCB.SAVER0:		.word 0		/* Special register used in kernel. */
TCB.SAVER1:		.word 0		/* Special register used in kernel. */
TCB.SAVESR:		.word 0		/* Status register. */
TCB.RETADDR:	.word 0		/* Return address, i.e. the resume	*/
							/* address of the task 				*/

TCB.SAVEREG:				/* The 14 registers interesting for context switch */
TCB.SAVEREG.LR:  .word 0
TCB.SAVEREG.SP:  .word 0
TCB.SAVEREG.R12: .word 0
TCB.SAVEREG.R11: .word 0
TCB.SAVEREG.R10: .word 0
TCB.SAVEREG.R9:  .word 0
TCB.SAVEREG.R8:  .word 0
TCB.SAVEREG.R7:  .word 0
TCB.SAVEREG.R6:  .word 0
TCB.SAVEREG.R5:  .word 0
TCB.SAVEREG.R4:  .word 0
TCB.SAVEREG.R3:  .word 0
TCB.SAVEREG.R2:  .word 0
TCB.SAVEREG.R1:  .word 0
TCB.SAVEREG.R0:  .word 0

TCB.TCBSIZE:	.space 1,0	/* Size marker of TCB */

#endif /* _ROSA_OFF_H_ */
