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

#include "system/list.h"

/***********************************************************
 * listCreate
 *
 * Comment:
 * Return FALSE upon failure
 **********************************************************/
//Create a list
void listCreate(List * list, void (*destroy)(void * data))
{
	list->size = 0;
	list->destroy = destroy;
	list->head = NULL;
	list->tail = NULL;
}

//Destroy list
void listDestroy(List * list)
{
	void * data;
	while(list->size > 0) {
		if((listRemove(list, NULL, (void **) &data) == 0) && (list->destroy != NULL)) {
			list->destroy(data);
		}
	}
	//Clean up
	memset(list, 0, sizeof(list));
}

//Insert an element after the listelement 'element'
int listInsert(List * list, Item * item, const void * data)
{
	Item * newItem;
	if((newItem = (Item *) malloc(sizeof(Item))) == NULL) {
		return -1;
	}
	newItem->data = (void *) data;
	if(item == NULL) {
		if(list->size == 0) {
			list->tail = newItem;
		}
		newItem->next = list->head;
		list->head = newItem;
	}
	else {
		if(item->next == NULL) {
			list->tail = newItem;
		}
		newItem->next = item->next;
		item->next = newItem;
	}
	list->size++;
	return 0;
}


//Remove an element after the item 'item'
int listRemove(List * list, Item * item, void ** data)
{
	Item * oldItem;
	if(list->size == 0) {
		return -1;
	}

	if(item == NULL) {
		*data = list->head->data;
		oldItem = list->head;
		list->head = list->head->next;
		if(list->size == 1) {
			list->tail = NULL;
		}
	}
	else {
		if(item->next == NULL) {
			return -1;
		}
		*data = item->next->data;
		oldItem = item->next;
		item->next = item->next->next;
		if(item->next == NULL) {
			list->tail = item;
		}
	}
	free(oldItem);
	list->size--;
	return 0;
}

//Get the list size
int listSize(List * list);

Item * listHead(List * list)
{
	return list->head;
}

Item * listTail(List * list)
{
	return list->tail;
}