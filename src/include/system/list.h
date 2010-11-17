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
/* Tab size: 4 */

#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>
#include <string.h>

#ifndef NULL
#define NULL 0
#endif

//List definition
typedef struct s_item {
	void * data;
	struct s_item * next;
} Item;

typedef struct list_ {
	int size;
	void (*destroy)(void * data);
	Item * head;
	Item * tail;
} List;

/***********************************************************
 * listCreate
 *
 * Comment:
 * Return FALSE upon failure
 **********************************************************/
//Create a list
void listCreate(List * list, void (*destroy)(void * data));

//Destroy list
void listDestroy(List * list);

//Insert an element after the listelement 'element'
int listInsert(List * list, Item * item, const void * data);

//Remove an element after the item 'item'
int listRemove(List * list, Item * item, void ** data);

//Get the list size
int listSize(List * list);
Item * listHead(List * list);
Item * listTail(List * list);

#endif /* _LIST_H_ */