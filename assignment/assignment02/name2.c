#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간
#define SORT_BY_NAME	0 // 이름 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		total_freq;		// 연도별 빈도 합
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName	*data;		// 이름 배열의 포인터
} tNames;

//임시이름 구조체 (읽어들인 한 줄 저장)
typedef struct
{
	char name[20];
	char sex;
}tempName;

////////////////////////////////////////////////////////////////////////////////
// 함수 원형 선언

// 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 이름과 성별에 의해 정렬 리스트(ordered list)를 유지해야 함 (qsort 함수 사용하지 않음)
// 1. 이미 등장한 이름인지 검사하기 위해
// 2. 새로운 이름을 삽입할 위치를 찾기 위해 binary_search 함수를 사용
// 새로운 이름을 저장할 메모리 공간을 확보하기 위해 memmove 함수를 이용하여 메모리에 저장된 내용을 복사
// names->capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
// start_year : 시작 연도 (2009)
void load_names( FILE *fp, int start_year, tNames *names);

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year);

// bsearch, qsort를 위한 비교 함수
// 정렬 기준 : 이름(1순위), 성별(2순위)
int compare_by_name( const void *n1, const void *n2);

// 정렬 기준 : 빈도 내림차순(1순위), 이름(2순위), 성별(3순위)
int compare_by_freq( const void *n1, const void *n2);

//이진탐색에 쓸 비교함
int compare_key( const void *n1, const void *n2);
// 이진탐색 함수
// found : key가 발견되는 경우 1, key가 발견되지 않는 경우 0
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, 
				int (*compare)(const void *, const void *), int *found);

////////////////////////////////////////////////////////////////////////////////
// 함수 정의

// 이름 구조체 초기화
// len를 0으로, capacity를 1로 초기화
// return : 구조체 포인터
tNames *create_names(void)
{
	tNames *pnames = (tNames *)malloc( sizeof(tNames));
	
	pnames->len = 0;
	pnames->capacity = 1000;
	pnames->data = (tName *)malloc(pnames->capacity * sizeof(tName));

	return pnames;
}

// 이름 구조체에 할당된 메모리를 해제
void destroy_names(tNames *pnames)
{
	free(pnames->data);
	pnames->len = 0;
	pnames->capacity = 0;

	free(pnames);
}

	
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tNames *names;
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

	// 이름 구조체 초기화
	names = create_names();
	
	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}
	
	// 연도별 입력 파일(이름 정보)을 구조체에 저장
	load_names( fp, 2009, names);
	
	fclose(fp);
	
	if (option == SORT_BY_NAME) {
		// 정렬 (이름순 (이름이 같은 경우 성별순))
		qsort( names->data, names->len, sizeof(tName), compare_by_name);
	}
	else { // SORT_BY_FREQ
		// 정렬 (빈도순 (빈도가 같은 경우 이름순))
		qsort( names->data, names->len, sizeof(tName), compare_by_freq);
	}
	
	// 이름 구조체를 화면에 출력
	print_names( names, MAX_YEAR_DURATION);
	
	// 이름 구조체 해제
	destroy_names( names);
	
	return 0;
}


//함수구현
void load_names( FILE *fp, int start_year, tNames *names)
{
	//지역변수 선언(파일 한 줄 내용)
	int year_local;
	char name_local[20];
	char sex_local;
	int freq_local;
	int found_var = -1;
	int index;
	int *found = &found_var;

	tName *name = names -> data; // name 포인터 생성

	tempName tempN;
	tempName *temp_name = &tempN; // 임시 포인터 생성 
	
	//반복문 읽고 집어넣기
	while(fscanf(fp, "%d %s %c %d", &year_local, name_local, &sex_local, &freq_local) == 4)
	{	
		//binary_search return값인 배열 index와 기존 이름, 성별 유무인 found정의	


		//처음은 그냥 넣기.
		if(names->len == 0)
		{
			strcpy(name -> name, name_local);
			name -> sex = sex_local;	
			name -> total_freq = freq_local;
			for(int i = 0; i < sizeof(name -> freq); i++)
				name -> freq[i] = 0;
			name -> freq[year_local - start_year] = freq_local;

			names -> len ++;

			continue;
		}

		//임시이름 구조체 값 대입
		strcpy(temp_name -> name, name_local);
		temp_name -> sex = sex_local;
		
		//binary_search로 index 얻어오기	
		index =	binary_search( temp_name, name, names->len, sizeof(tName), compare_key, found);


		if(*found == 1)
		{
			(name + index) -> freq[year_local - start_year] = freq_local;
            (name + index) -> total_freq += freq_local;
		}
		else if( *found == 0 )
		{	
			(names -> len)++;

			//저장공간 확인 후 부족하면 용량 1000 늘리기	
			if(names -> len == names->capacity)
			{	
			    (names->capacity) += 1000;
			    tName *newData = (tName *)realloc(names->data, names->capacity * sizeof(tName));
			    if (!newData)
			    {
			        // realloc에 실패한 경우의 처리를 추가
			        fprintf(stderr, "Memory allocation error\n");
			        exit(1);
			    }
			    names->data = newData;
			    name = newData;  // name 포인터 업데이트
			}


			//name 주소 한칸 옮기기와 정보 입력
			memmove((name + index + 1), (name + index), sizeof(tName) * (names->len - index - 1));

			strcpy((name + index) -> name, name_local);
			(name + index) -> sex = sex_local;
			(name + index) -> total_freq = freq_local;
			for (int i = 0; i < MAX_YEAR_DURATION; i++)
                (name + index) -> freq[i] = 0;
			(name + index) -> freq[year_local - start_year] = freq_local;
		}

	}



}

void print_names( tNames *names, int num_year)
{

	tName *location;
	location = names->data;

	for(int i = 0; i < names -> len ; i++ )
		{
			printf("%s\t%c\t%d\t",location->name, location->sex, location->total_freq);
			
		for(int j = 0; j < num_year; j++)
		{	
			if (j < num_year - 1) 
			{ 	
				printf("%d\t", location->freq[j]);
			}
			else
			{
				printf("%d", location->freq[j]);
			}
		}


			printf("\n");

			location++;
		}


}

int compare_by_name( const void *n1, const void *n2)
{
	tName *N1 = (tName *)n1;
	tName *N2 = (tName *)n2;

	if (strcmp(N1->name, N2 -> name) == 0)
	{
		if (N1->sex < N2->sex)
			return -1;
		else
			return 1;
	}
	else if (strcmp(N1->name, N2 -> name) < 0)
		return -1;
	else
		return 1;

}
// 정렬 기준 : 빈도 내림차순(1순위), 이름(2순위), 성별(3순위)
int compare_by_freq( const void *n1, const void *n2)
{
	tName *N1 = (tName *)n1;
	tName *N2 = (tName *)n2;

	if (N1->total_freq == N2 -> total_freq)
	{
		if (strcmp(N1->name, N2 -> name) == 0)
		{
			if (N1->sex < N2->sex)
				return -1;
		else
			return 1;
		}
		else if (strcmp(N1->name, N2 -> name) < 0)
			return -1;
		else
			return 1;
	}
	else if (N1->total_freq < N2->total_freq)
		return 1;
	else
		return -1;

	return 0;


}

int compare_key( const void *n1, const void *n2)
{
	tempName *N1 = (tempName *)n1;
	tName *N2 = (tName *)n2;

	if (strcmp(N1->name, N2 -> name) == 0)
	{
		if (N1->sex == N2->sex)
			return 0;
		else if (N1->sex < N2->sex)
			return -1;
		else return 1;
	}
	else if (strcmp(N1->name, N2 -> name) < 0)
		return -1;
	else
		return 1;

}
// 이진탐색 함수
// found : key가 발견되는 경우 1, key가 발견되지 않는 경우 0
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, 
				int (*compare)(const void *, const void *), int *found)
{
	//왼쪽 오른쪽
	int l, r, m;
	l = 0;
	r = nmemb-1;
	m = 0;
	while(l <= r)
	{

		m = (l+r) >> 1;

		if(compare(key, base + m * size) == 0)
			{
				*found = 1;
				return m;
			}
		else if(compare(key, base + m * size) < 0)
			{
				r = m -1;
			}
		else if(compare(key, base + m * size) > 0)
			{
				l = m + 1;
			}
	}

	*found = 0;

	return l;
}