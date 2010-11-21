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

#ifndef _HEAP_H_
#define _HEAP_H_

typedef struct Heap_ {
	int size;
	int (*compare)(const void * key1, const void * key2);
	void (*destroy)(void * data);
	void ** tree;
} Heap;

void heapInit(Heap * heap, int (*compare)(const void * key1, const void * key2), void (*destroy)(void * data));
void heapDestroy(Heap * heap);
int heapInsert(Heap * heap, const void * data);
int heapExtract(Heap * heap, void ** data);
#define heapSize(heap) ((heap)->size)

#define heapPeek(heap) ((heap)->tree == NULL ? NULL : (heap)->tree[0])

#endif /* _HEAP_H_ */

