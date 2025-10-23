#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define SEARCH			4
#define DELETE			5
#define COUNT			6

// User structure type definition
typedef struct 
{
	char	*name;	// 이름
	char	sex;	// 성별 M or F
	int		freq;	// 빈도
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tName		*dataPtr;
	struct node	*llink;
	struct node	*rlink;
} NODE;

typedef struct
{
	int		count;
	NODE	*head;
	NODE	*rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void);

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList);

// Inserts data into list
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
int addNode( LIST *pList, tName *dataInPtr);

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, tName *keyPtr, tName **dataOutPtr);

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchNode( LIST *pList, tName *pArgu, tName **dataOutPtr);

// returns number of nodes in list
int countList( LIST *pList);

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList);

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const tName *));

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const tName *));

// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr);

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tName **dataOutPtr);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu);

////////////////////////////////////////////////////////////////////////////////
// Allocates dynamic memory for a name structure, initialize fields(name, sex, freq) and returns its address to caller
// return	name structure pointer
//			NULL if overflow
tName *createName( char *name, char sex, int freq); 

// Deletes all data in name structure and recycles memory
void destroyName( tName *pNode);

////////////////////////////////////////////////////////////////////////////////
// gets user's input
int get_action()
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch);
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'P':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'S':
			return SEARCH;
		case 'D':
			return DELETE;
		case 'C':
			return COUNT;
	}
	return 0; // undefined action
}

// compares two names in name structures
// for _search function
int cmpName( const tName *pName1, const tName *pName2)
{
	int ret = strcmp( pName1->name, pName2->name);
	if (ret != 0) return ret;

	return pName1->sex - pName2->sex;
}

// prints contents of name structure
// for traverseList and traverseListR functions
void print_name(const tName *dataPtr)
{
	printf( "%s\t%c\t%d\n", dataPtr->name, dataPtr->sex, dataPtr->freq);
}

// gets user's input (name/sex)
void input_name(char *name, char *sex)
{
	char *p;

	while (1)
	{
		fprintf( stderr, "Input a name/sex to find: ");
		fscanf( stdin, "%s", name);
				
		if ((p = strchr(name, '/')) == NULL)
		{
			fprintf( stderr, "invalid name/sex!\n");
			continue;
		}
		*p++ = 0;
		if (*p && (*p == 'M' || *p == 'F')) *sex = *p;
		else
		{
			fprintf( stderr, "invalid sex! (M | F)\n");
			continue;
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	
	char name[100];
	char sex;
	int freq;
	
	tName *pName;
	int ret;
	FILE *fp;
	
	if (argc != 2){
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	while(fscanf( fp, "%s\t%c\t%d", name, &sex, &freq) != EOF)
	{
		pName = createName( name, sex, freq);
		
		ret = addNode( list, pName);
		
		if (ret == 0 || ret == 2) // failure or duplicated
		{
			destroyName( pName);
		}
	}
	
	fclose( fp);
	
	fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	
	while (1)
	{
		tName *ptr;
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list);
				return 0;
			
			case FORWARD_PRINT:
				traverseList( list, print_name);
				break;
			
			case BACKWARD_PRINT:
				traverseListR( list, print_name);
				break;
			
			case SEARCH:
				input_name(name, &sex);
				
				pName = createName( name, sex, 0);

				if (searchNode( list, pName, &ptr)) print_name( ptr);
				else fprintf( stdout, "%s/%c not found\n", name, sex);
				
				destroyName( pName);
				break;
				
			case DELETE:
				input_name(name, &sex);
				
				pName = createName( name, sex, 0);

				if (removeNode( list, pName, &ptr))
				{
					//fprintf( stdout, "(%s/%c, %d) deleted\n", ptr->name, ptr->sex, ptr->freq);
					//destroyName( ptr);
				}
				else fprintf( stdout, "%s/%c not found\n", name, sex);
				
				destroyName( pName);
				break;
			
			case COUNT:
				fprintf( stdout, "%d\n", countList( list));
				break;
		}
		
		if (action) fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////함 수    구 현//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

LIST *createList(void){
	LIST* plist;
	plist = (LIST *)malloc(sizeof(LIST));

	return plist;
}

void destroyList( LIST *pList){
	
	NODE *pLoc = pList->head;

	for(int i = 0; i < pList -> count; i++){
		destroyName(pLoc->dataPtr);
		pLoc = pLoc->llink;
	}

	free(pList);

}

// Inserts data into list
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
//수정해야됨, 서치 넣고, pPre 탐색 필요. rlink 도 연결필요.
int addNode( LIST *pList, tName *dataInPtr){
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
int removeNode( LIST *pList, tName *keyPtr, tName **dataOutPtr){

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
int searchNode( LIST *pList, tName *pArgu, tName **dataOutPtr){
	int found;

	NODE *pPre;
	NODE *pLoc;

	found = _search(pList, &pPre, &pLoc, pArgu);
	if (found == 0) return 0;

	*dataOutPtr = pLoc -> dataPtr;

	return 1;
}

// returns number of nodes in list
int countList( LIST *pList){
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
int emptyList( LIST *pList){
	if(pList->head == NULL) return 1;

	return 0;
}

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const tName *)){
	NODE* pLoc = pList -> head;

	while(pLoc != NULL){

		callback(pLoc->dataPtr);
		pLoc = pLoc -> llink;		
	}
	
}

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const tName *)){
		NODE* pLoc = pList -> rear;

	while(pLoc != NULL){

		callback(pLoc->dataPtr);
		pLoc = pLoc -> rlink;		
	}
	
}

// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr){
	NODE *pNew;
	pNew = (NODE*)malloc(sizeof(NODE));

	pNew->dataPtr = dataInPtr;

	if(pPre == NULL){
		pNew->llink = pList -> head;
		pList->head = pNew;
	}
	else{
		pNew->llink = pPre;
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
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tName **dataOutPtr){
	
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
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu){
	int cmp;

	*pLoc = pList -> head;
	*pPre = NULL;

	while(*pLoc != NULL && (cmp = cmpName(pArgu, (*pLoc)->dataPtr)) < 0 ){
		 *pPre = *pLoc;
		 *pLoc = (*pLoc)->llink;
	}

	if (cmp == 0) return 1;
	else return 0;

}

////////////////////////////////////////////////////////////////////////////////
// Allocates dynamic memory for a name structure, initialize fields(name, sex, freq) and returns its address to caller
// return	name structure pointer
//			NULL if overflow
tName *createName( char *name, char sex, int freq){
	tName* pName;

	pName = (tName*)malloc(sizeof(tName));

	pName -> name = strdup(name);
	pName -> sex = sex;
	pName -> freq = freq;
}

// Deletes all data in name structure and recycles memory
void destroyName( tName *pNode){
	free(pNode->name);
	free(pNode);
}

