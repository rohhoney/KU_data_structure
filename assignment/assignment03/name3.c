#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간
#define SORT_BY_NAME	0 // 이름 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// 이름 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		total_freq;		// 연도별 빈도 합
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tName		*dataPtr;
	struct node	*link; // 이름순 리스트를 위한 포인터
	struct node	*link2; // 빈도순 리스트를 위한 포인터
} NODE;

typedef struct
{
	int		count;
	NODE	*head; // 이름순 리스트의 첫번째 노드에 대한 포인터
	NODE	*head2; // 빈도순 리스트의 첫번째 노드에 대한 포인터
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void); 

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList);

// internal insert function
// inserts data into a new node
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr); 

// internal function
// connects node into a frequency list
static void _link_by_freq( LIST *pList, NODE *pPre, NODE *pLoc);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu);

static int _search_by_freq( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu);

// 이름 구조체를 위한 메모리를 할당하고, 이름(name)과 성별(sex)을 초기화
// return	할당된 이름 구조체에 대한 pointer
//			NULL if overflow
tName *createName( char *name, char sex); 

//  이름 구조체에 할당된 메모리를 해제
void destroyName( tName *pNode);

////////////////////////////////////////////////////////////////////////////////
// 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 리스트에 저장
// 이미 리스트에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 리스트에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함
// start_year : 시작 연도 (2009)
void load_names( FILE *fp, int start_year, LIST *list);

// 이름순 리스트를 순회하며 빈도 순 리스트로 연결
void connect_by_frequency(LIST *list);

// 이름 리스트를 화면에 출력
void print_names( LIST *pList, int num_year); // 이름순
void print_names_by_freq( LIST *pList, int num_year); // 빈도순

////////////////////////////////////////////////////////////////////////////////
// compares two names in name structures
// for _search function
// 정렬 기준 : 이름(1순위), 성별(2순위)
static int compare_by_name(const tName *pName1, const tName *pName2)
{
	int ret = strcmp( pName1->name, pName2->name);
	if (ret != 0) return ret;

	return pName1->sex - pName2->sex;
}

// for _search_by_freq function
// 정렬 기준 : 빈도 내림차순(1순위), 이름(2순위), 성별(3순위)
static int compare_by_freq(const tName *pName1, const tName *pName2)
{
	int ret = pName2->total_freq - pName1->total_freq;
	if (ret != 0) return ret;
	
	return compare_by_name(pName1, pName2);
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	int option;
	FILE *fp;
	
	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s option FILE\n\n", argv[0]);
		fprintf( stderr, "option\n\t-n\t\tsort by name\n\t-f\t\tsort by frequency\n");
		return 1;
	}

	if (strcmp( argv[1], "-n") == 0) option = SORT_BY_NAME;
	else if (strcmp( argv[1], "-f") == 0) option = SORT_BY_FREQ;
	else {
		fprintf( stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}

	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	// 입력 파일로부터 이름 정보를 리스트에 저장 (이름순 리스트 구축)
	load_names( fp, 2009, list);
	
	fclose( fp);

	if (option == SORT_BY_NAME) {
		
		// 이름순 리스트를 화면에 출력
		print_names( list, MAX_YEAR_DURATION);
	}
	else { // SORT_BY_FREQ
	
		// 빈도순 리스트 연결
		connect_by_frequency( list);
		
		// 빈도순 리스트를 화면에 출력
		print_names_by_freq( list, MAX_YEAR_DURATION);
	}
	
	// 이름 리스트 메모리 해제
	destroyList( list);
	
	return 0;
}

///////////////////////함수 구현 하기//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void) //완성 
{
	LIST *plist;

	plist = (LIST*)malloc(sizeof(LIST));

	plist -> count = 0;
	plist -> head = NULL;
	plist -> head2 = NULL;


	return plist;
}

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList) //완성
{
	NODE *pPre = NULL;
	NODE *pLoc = pList->head;

	while(pLoc != NULL)
	{
		destroyName(pLoc->dataPtr);
		pPre = pLoc;
		pLoc = pLoc -> link;
		free(pPre);
	}

	free(pList);

}

// internal insert function
// inserts data into a new node
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr) //완성
{	

	NODE * pNew = (NODE*)malloc(sizeof(NODE));
	if(pNew == NULL) return 0;

	pNew -> dataPtr = dataInPtr;

	if(pPre == NULL) 
	{	
		pNew -> link = pList ->  head;
		pList -> head = pNew;

	}
	else
	{
		pNew -> link = pPre -> link;
		pPre -> link = pNew;
	}



	return 1;

}

// internal function
// connects node into a frequency list
static void _link_by_freq( LIST *pList, NODE *pPre, NODE *pLoc)
{

	if(pPre == NULL) 
	{	
		pLoc -> link2 = pList -> head2;
		pList -> head2 = pLoc;

	}
	else
	{
		pLoc -> link2 = pPre -> link2;
		pPre -> link2 = pLoc;
	}


}

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu) //완성
{
	int found;

	*pPre = NULL;
	*pLoc = pList->head;


	while(*pLoc != NULL && (found = compare_by_name(pArgu, (*pLoc)->dataPtr)) > 0 )
	{
		*pPre = *pLoc;
		*pLoc = (*pLoc) -> link;
	}

	if(*pLoc == NULL) return 0;

	if (found == 0) return 1;
	if (found < 0) return 0;

}

static int _search_by_freq( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu)
{

	int found;

	*pPre = NULL;
	*pLoc = pList->head2;


	while(*pLoc != NULL && (found = compare_by_freq(pArgu, (*pLoc)->dataPtr)) > 0 )
	{
		*pPre = *pLoc;
		*pLoc = (*pLoc) -> link2;
	}

	if(*pLoc == NULL) return 0;

	if (found == 0) return 1;
	if (found < 0) return 0;


}

// 이름 구조체를 위한 메모리를 할당하고, 이름(name)과 성별(sex)을 초기화
// return	할당된 이름 구조체에 대한 pointer
//			NULL if overflow
tName *createName( char *name, char sex) //완성 
{
	tName *created_Name;
	created_Name = (tName*)malloc(sizeof(tName));

	strcpy(created_Name->name, name);
	created_Name -> sex = sex;


	return created_Name;

}

//  이름 구조체에 할당된 메모리를 해제
void destroyName( tName *pNode) //완성 
{
	free(pNode);
}

////////////////////////////////////////////////////////////////////////////////
// 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 리스트에 저장
// 이미 리스트에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 리스트에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함
// start_year : 시작 연도 (2009)
void load_names( FILE *fp, int start_year, LIST *list) //완성 
{
	int year_local;
	char name_local[20];
	char sex_local;
	int freq_local;
	int adress = 0;
	int found;

	while(fscanf(fp, "%d %s %c %d", &year_local, name_local, &sex_local, &freq_local) == 4)
	{
		tName * name = createName(name_local, sex_local);
		if(!name){fprintf(stderr,"Cannot create Name");  return;}

		NODE *pPre;
		NODE *pLoc;

		found = _search( list, &pPre, &pLoc, name);
		if(found)//찾음
		{
			pLoc -> dataPtr -> total_freq += freq_local;
			pLoc -> dataPtr -> freq[year_local - start_year] = freq_local;
		}
		else // 못찾음 
		{
			
			int insert_check;

			insert_check = _insert(list, pPre, name);
			
			if (!insert_check)
				{fprintf(stderr, "Cannot create Node"); return;}

			list->count++;

			if (pPre == NULL)
			{
			for(int i = 0; i < MAX_YEAR_DURATION; i++)
				list->head -> dataPtr -> freq[i] = 0;
			list->head -> dataPtr ->total_freq = freq_local;
			list->head -> dataPtr -> freq[year_local - start_year] = freq_local;
			}
			else
			{
				for(int i = 0; i < MAX_YEAR_DURATION; i++)
				pPre-> link -> dataPtr -> freq[i] = 0;
			pPre-> link -> dataPtr ->total_freq = freq_local;
			pPre-> link -> dataPtr -> freq[year_local - start_year] = freq_local;

			}
		}

	}

}

// 이름순 리스트를 순회하며 빈도 순 리스트로 연결
void connect_by_frequency(LIST *list)
{
	NODE *anode = list->head;
	
	for(int i = 0; i < list -> count; i++)
	{
		NODE *pPre;
		NODE *pLoc;


		int found = _search_by_freq( list, &pPre, &pLoc, anode -> dataPtr);

		_link_by_freq(list, pPre, anode);

		anode = anode -> link;
	}
}
// 이름 리스트를 화면에 출력
void print_names( LIST *pList, int num_year) //완성 
{
	NODE *pLoc = pList->head;

	while(pLoc != NULL)
		{
			printf("%s\t%c\t%d\t",pLoc->dataPtr->name, pLoc->dataPtr->sex, pLoc->dataPtr->total_freq);
			
			for(int j = 0; j < num_year; j++)
			{	
				if (j < num_year - 1) 
				{
					printf("%d\t", pLoc->dataPtr->freq[j]);
				}
				else
				{
					printf("%d", pLoc->dataPtr->freq[j]);
				}
			}

			printf("\n");

			pLoc = pLoc->link;


		}
}
void print_names_by_freq( LIST *pList, int num_year)
{
		NODE *pLoc = pList->head2;

	while(pLoc != NULL)
		{
			printf("%s\t%c\t%d\t",pLoc->dataPtr->name, pLoc->dataPtr->sex, pLoc->dataPtr->total_freq);
			
			for(int j = 0; j < num_year; j++)
			{	
				if (j < num_year - 1) 
				{
					printf("%d\t", pLoc->dataPtr->freq[j]);
				}
				else
				{
					printf("%d", pLoc->dataPtr->freq[j]);
				}
			}

			printf("\n");

			pLoc = pLoc->link2;


		}
} 
