#include <stdlib.h> // malloc

#include "adt_dlist.h"

// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr);
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr);
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu);

static int _insert( LIST *pList, NODE *pPre, void *dataInPtr){
	NODE *pNew;
	pNew = (NODE*)malloc(sizeof(NODE));

	pNew->dataPtr = dataInPtr; //dataPtr는 void포인터

	if(pPre == NULL){
		pNew->llink = pList -> head;
		pList->head = pNew;
	}
	else{
		pNew->llink = pPre->llink;
		pPre->llink = pNew;
	}

	if(pNew->llink == NULL){
		pNew -> rlink = pList -> rear;
		pList -> rear = pNew;

	}
	else{
		pNew->rlink = pNew->llink->rlink;
		pNew->llink->rlink = pNew;
	}

	pList->count ++;

	if (pNew == NULL) return 0;
	return 1;
}

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr){

	*dataOutPtr = pLoc -> dataPtr;

	if(pPre == NULL){
		pList -> head = pLoc -> llink;
	}
	else{

		pPre -> llink = pLoc -> llink;
	}

	if(pLoc->llink == NULL){
		pList -> rear = pPre;
	}
	else{
		pLoc->llink->rlink = pPre;
	}

	free(pLoc);

}

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu){

	int cmp;

	*pLoc = pList -> head;
	*pPre = NULL;

	while((*pLoc) != NULL && (cmp = pList -> compare(pArgu, (*pLoc)->dataPtr)) > 0 ){
		 *pPre = *pLoc;
		 *pLoc = (*pLoc)->llink;
	}

	if(*pLoc == NULL) return 0;
	else if (cmp == 0) return 1;
	else return 0;
}

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList( int (*compare)(const void *, const void *))
{
	LIST* pList;
	pList = (LIST *)malloc(sizeof(LIST));
	
	pList -> head = NULL;
	pList -> rear = NULL;

	pList -> compare = compare;
	
	return pList;
}

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList, void (*callback)(void *))
{

	NODE *pPre = NULL;
	NODE *pLoc = pList->head;

	while(pLoc != NULL)
	{
		callback(pLoc->dataPtr);
		pPre = pLoc;
		pLoc = pLoc -> llink;
		free(pPre);
	}

	free(pList);

}

// Inserts data into list
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
int addNode( LIST *pList, void *dataInPtr)
{
	int found;
	int success;
	NODE *pPre;
	NODE *pLoc;

	found = _search(pList, &pPre, &pLoc, dataInPtr);
	if(found == 1) return 2;

	success = _insert(pList, pPre, dataInPtr);

	if(success == 0) return 0;
	else return 1;
}

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, void *keyPtr, void **dataOutPtr)
{
	int found;

	NODE *pPre;
	NODE *pLoc;

	found = _search(pList, &pPre, &pLoc, keyPtr);
	if (found == 0) return 0;

	_delete(pList, pPre, pLoc, dataOutPtr); // 잠시확인


	return 1;
}

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchNode( LIST *pList, void *pArgu, void **dataOutPtr)
{
	int found;

	NODE *pPre;
	NODE *pLoc;

	found = _search(pList, &pPre, &pLoc, pArgu);
	if (found == 0) return 0;

	*dataOutPtr = pLoc -> dataPtr;

	return 1;
}

// returns number of nodes in list
int countList( LIST *pList)
{
	int cnt = 0;
	NODE *pLoc = pList->head;


	while(pLoc != NULL)
	{
		cnt++;
		pLoc = pLoc->llink;
	}

	return cnt;
}

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList)
{
	if(pList->head == NULL) return 1;

	return 0;
}

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const void *))
{
	NODE* pLoc = pList -> head;

	while(pLoc != NULL)
	{
		callback(pLoc->dataPtr);
		pLoc = pLoc -> llink;	
	}
	
}

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const void *))
{
	NODE* pLoc = pList -> rear;

	while(pLoc != NULL)
	{
		callback(pLoc->dataPtr);
		pLoc = pLoc -> rlink;		
	}

}