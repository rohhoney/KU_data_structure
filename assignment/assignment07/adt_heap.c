#include <stdio.h>
#include <stdlib.h> // malloc, realloc, free

#include "adt_heap.h"

static void _reheapUp( HEAP *heap, int index);
static void _reheapDown( HEAP *heap, int index);


/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
The initial capacity of the heap should be 10
*/
HEAP *heap_Create( int (*compare) (const void *arg1, const void *arg2)){
	HEAP *pHeap;
	pHeap = (HEAP*)malloc(sizeof(HEAP));

	pHeap -> last = -1;	// 힙이 비어있을 때
	pHeap -> capacity = 10;
	pHeap -> heapArr = (void**)malloc(sizeof(void*) * pHeap->capacity);
	pHeap -> compare = compare;

	return pHeap;
}

/* Free memory for heap
*/
void heap_Destroy( HEAP *heap, void (*remove_data)(void *ptr)){
	while(heap->last != -1){

		remove_data(*(heap->heapArr + heap->last));
		heap -> last --;
	}

	free(heap->heapArr);
	free(heap);
}

/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heap_Insert( HEAP *heap, void *dataPtr){
	
	if (heap->last + 1 == heap->capacity){
		heap->capacity += 100;
		heap->heapArr = (void**)realloc(heap->heapArr, sizeof(void*) * heap->capacity);
	}
	if(heap->heapArr == NULL)
		return 0;

	heap->last ++;

	int index = heap -> last;

	*(heap -> heapArr + index) = dataPtr;

	_reheapUp(heap, index);

	return 1;

}

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heap_Delete( HEAP *heap, void **dataOutPtr){
	void *temp;

	if(heap->last == -1){
		return 0;
	}

	*dataOutPtr = *(heap -> heapArr);
	
	//exchange root, last
	temp = *(heap->heapArr);
	*(heap->heapArr)= *(heap->heapArr + heap->last);
	*(heap->heapArr + heap -> last) = temp;

	heap->last --;
	
	_reheapDown(heap, 0);



	return 1;
}

/*
return 1 if the heap is empty; 0 if not
*/
int heap_Empty(  HEAP *heap){
	if(heap -> last == -1)
		return 1;
	else
		return 0;
}

/* Print heap array */
void heap_Print( HEAP *heap, void (*print_func) (const void *data)){
	int loc = 0;

	while(loc <= heap->last){
		print_func(*(heap->heapArr + loc));
		loc++;
	}
	printf("\n");
}





/* Reestablishes heap by moving data in child up to correct location heap array
*/
static void _reheapUp( HEAP *heap, int index){
	int parent_index;
	void *temp;

	if(index != 0){
		parent_index = (index-1)/2;
		if(heap -> compare(*(heap->heapArr + index), *(heap->heapArr + parent_index)) > 0){
			
			//exchange newNode, parent
			temp = *(heap->heapArr + index);
			*(heap->heapArr + index)= *(heap->heapArr + parent_index);
			*(heap->heapArr + parent_index) = temp;

			_reheapUp(heap, parent_index);

		}
	}
}

/* Reestablishes heap by moving data in root down to its correct location in the heap
*/
static void _reheapDown( HEAP *heap, int index){
	int left;
	int right;
	int largest;
	void *temp;

	//if left child exist
	if(index * 2 + 1 <= heap->last){
		left = index * 2 + 1;

		//if right child exits
		if(index *2 + 2 <= heap -> last){
			right = index * 2 + 2;
		}
		else{

			right = -1;
		}

		if(right == -1){
			largest = left;
		}
		else if(heap->compare(*(heap->heapArr + left), *(heap->heapArr + right)) >= 0){
			
			largest = left;

		}
		else{

			largest = right;
		}

		if(heap-> compare(*(heap->heapArr + largest), *(heap->heapArr + index))> 0){

			//exchange root, largest child
			temp = *(heap->heapArr + index);
			*(heap->heapArr + index) = *(heap->heapArr + largest);
			*(heap->heapArr + largest) = temp;

			_reheapDown(heap, largest);
		}
	}





}







