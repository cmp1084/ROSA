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
#ifndef _ROSA_SYSTICK_H_
#define _ROSA_SYSTICK_H_

#include "kernel/rosa_ext.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_scheduler.h"
#include "system/heap.h"

/***********************************************************
 * _sysTickIncrease
 *
 * Comment:
 * Increase the systick by one.
 **********************************************************/
void _sysTickIncrease(void);

/***********************************************************
 * ROSA_getSysTick
 *
 * Comment:
 * Return the number of systicks passed.
 *
 * Out:
 * unsigned int - The value of systick
 **********************************************************/
unsigned int ROSA_sysTickGet(void);

/***********************************************************
 * _resetSysTick
 *
 * Comment:
 * Reset the sysTick.
 **********************************************************/
void _sysTickReset(void);

/***********************************************************
 * ROSA_wait
 *
 * Comment:
 * Wait a specific number of system ticks
 *
 * In:
 * unsigned int ticks - the number of ticks to wait.
 *
 **********************************************************/
void ROSA_wait(unsigned int ticks);

/***********************************************************
 * ROSA_waitUntil
 *
 * Comment:
 * Wait until the absolute system tick specified by the
 * parameter.
 *
 * In:
 * unsigned int absoluteTick - the absolute tick to wait for.
 *
 **********************************************************/
void ROSA_waitUntil(unsigned int absoluteTick);

#endif /* _ROSA_SYSTICK_H_ */