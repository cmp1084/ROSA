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
 * Log:
 * Date             Ver     History
 * 20101113         1.x     Adding prio, priority.
 * 20100728         1.0     First AVR32 version, Marcus Jansson
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
 * These are the offsets ROSA low level routines uses to
 * reach into the Tcb struct.
 *
 * Keep in sync with rosa_tcb.h
 *
 **********************************************************/
.offset  0
TCB.STARTMARK:
TCB.ID:         .word 0     /* Task identifier, 4 ASCII characters */
TCB.STADDR:     .word 0     /* Startaddress of taskcode            */
TCB.DATAAREA:   .word 0     /* Startaddress of dataarea (stack)    */
TCB.DATASIZE:   .word 0     /* Size of dataarea, used in initialization */
                            /* address of the task                 */
TCB.PRIO:       .word 0     /* Priority */
TCB.WAITUNTIL:  .word 0     /* Wait until this sysTick before moving into ready state */
TCB.SAVESR:     .word 0     /* Status register.                    */
TCB.SAVEREG:				/* The 14 registers interesting for context switch */
TCB.SAVEREG.PC:  .word 0
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
TCB.TCBSIZE:	.word 0	/* Size marker of TCB */

#endif /* _ROSA_OFF_H_ */
