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

#ifndef rosa_EXT_H_
#define rosa_EXT_H_

#include "rosa_def.h"

/***********************************************************
 * Global variables
 ***********************************************************/
extern tcb * TCBLIST;
extern tcb * EXECTASK;


/***********************************************************
 * Kernel low level context switch functions
 ***********************************************************/
extern void ROSA_contextInit(tcb * tcbTask);
extern void ROSA_contextRestore(void);
extern void ROSA_contextSave(void);
extern void ROSA_contextSaveFromISR(void);
extern void ROSA_contextRestoreFromISR(void);


/***********************************************************
 * Other kernel functions
 ***********************************************************/
//Kernel lowlevel initialization
extern void ROSA_init_lowlevel(void);

//This is the lowlevel call to start the first task.
//EXECTASK should have been preloaded with the first task to run.
extern void ROSA_start_lowlevel(void);

#endif /* rosa_EXT_H_ */
