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
#include <stdlib.h>
#include <string.h>

#include "system/heap.h"

//Private defines
#define heapParent(npos) ((int)(((npos) - 1) / 2))
#define heapLeft(npos) (((npos) * 2) + 1)
#define heapRight(npos) (((npos) * 2) + 2)


void heapInit(Heap * heap, int (*compare)(const void * key1, const void * key2), void (*destroy)(void * data))
{
	heap->size = 0;
	heap->compare = compare;
	heap->destroy = destroy;
	heap->tree = NULL;
}

void heapDestroy(Heap * heap)
{
	int i;
	if(heap->destroy != NULL) {
		for(i = 0; i < heapSize(heap); ++i) {
			heap->destroy(heap->tree[i]);
		}
	}
	free(heap->tree);
	memset(heap, 0, sizeof(Heap));
}

int heapInsert(Heap * heap, const void * data)
{
	void * temp;
	int ipos, ppos;

	if((temp = (void **)realloc(heap->tree, (heapSize(heap) + 1) * sizeof(void *))) == NULL) {
		return -1;
	}
	else {
		heap->tree = temp;
	}
	heap->tree[heapSize(heap)] = (void *)data;
	ipos = heapSize(heap);
	ppos = heapParent(ipos);
	while((ipos > 0) && (heap->compare(heap->tree[ppos], heap->tree[ipos]) < 0)) {
		temp = heap->tree[ppos];
		heap->tree[ppos] = heap->tree[ipos];
		heap->tree[ipos] = temp;
		ipos = ppos;
		ppos = heapParent(ipos);
	}
	heap->size++;
	return 0;
}

int heapExtract(Heap * heap, void ** data)
{
	void * save, * temp;
	int ipos, lpos, rpos, mpos;

	if(heapSize(heap) == 0) {
		return -1;
	}

	*data = heap->tree[0];
	save = heap->tree[heapSize(heap) - 1];
	if((heapSize(heap) - 1) > 0) {
		if((temp = (void **)realloc(heap->tree, (heapSize(heap) - 1) * sizeof(void *))) == NULL) {
			return -1;
		}
		else {
			heap->tree = temp;
		}
		heap->size--;
	}
	else {
		free(heap->tree);
		heap->tree = NULL;
		heap->size = 0;
		return 0;
	}
	heap->tree[0] = save;
	ipos = 0;
	lpos = heapLeft(ipos);
	rpos = heapRight(ipos);

	while(1) {
		lpos = heapLeft(ipos);
		rpos = heapRight(ipos);
		if((lpos < heapSize(heap)) && (heap->compare(heap->tree[lpos], heap->tree[ipos]) > 0)) {
			mpos = lpos;
		}
		else {
			mpos = ipos;
		}

		if((rpos < heapSize(heap)) && (heap->compare(heap->tree[rpos], heap->tree[mpos]) > 0)) {
			mpos = rpos;
		}

		if(mpos == ipos) {
			break;
		}
		else {
			temp = heap->tree[mpos];
			heap->tree[mpos] = heap->tree[ipos];
			heap->tree[ipos] = temp;
			ipos = mpos;
		}
	}
	return 0;
}

/*

int prioQueuePut(Heap * heap, const Item * item)
{
	Item * data;

	if((data = (Item *)malloc(Item))) == NULL) {
		return -1;
	}
	memcpy(data, item, sizeof(Item));
	if(heapInsert(heap, data) != 0) {
		return -1;
	}
	return 0;
}

int prioQueueGet(Heap * heap, Item *item)
{
	Item * data;

	if(heapSize(heap) == 0) {
		return -1;
	}
	else {
		if(heapExtract(heap, (void **)&data) != 0) {
			return -1;
		}
		else {
			memcpy(item, data, sizeof(Item));
			free(data);
		}
	}
	return 0;
}

*/