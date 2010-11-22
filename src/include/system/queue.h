/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// ///''//,
             ,///' '///,'/////',/////'  /////'\\,

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
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdlib.h>
#include "system/list.h"

typedef List Queue;
#define queueCreate listCreate
#define queueDestroy listDestroy
#define queueSize listSize

int queueAdd(Queue * queue, const void * data);
int queueRemove(Queue * queue, const void ** data);
void * queuePeek(const Queue * queue);

#endif /* _QUEUE_H_ */