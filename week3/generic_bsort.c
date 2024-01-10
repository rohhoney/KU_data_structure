#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//함수선언 부분
void bsort(void *base, size_t nmemb, size_t size, 
						int (*compar)(const void *, const void *));
int compar_int(const void *elem1, const void *elem2);
int compar_float(const void *elem1, const void *elem2);
int compar_char(const void *elem1, const void *elem2);

void print_int(void *elem);
void print_float(void *elem);
void generic_print_arr(void *base, size_t nmemb, size_t size,void (*print)(void*));
void print_char(void *elem);

int main(void)
{
	int arr[5] = {1, 4, 2, 3, 9};
	float arrf[6] = {1.1, 1.2, 3.2, 2.3, 6.2, 3.5};
	char arrc[4] = {'b', 's', 'c', 'q'};
	int *a = arr;

	bsort(arr, sizeof(arr) / sizeof(arr[0]), sizeof(arr[0]), compar_int);
	generic_print_arr(arr, sizeof(arr) / sizeof(arr[0]), sizeof(arr[0]), print_int);

	bsort(arrf, sizeof(arrf) / sizeof(arrf[0]), sizeof(arrf[0]), compar_float);
	generic_print_arr(arrf, sizeof(arrf) / sizeof(arrf[0]), sizeof(arrf[0]), print_float);

	bsort(arrc, sizeof(arrc) / sizeof(arrc[0]), sizeof(arrc[0]), compar_char);
	generic_print_arr(arrc, sizeof(arrc) / sizeof(arrc[0]), sizeof(arrc[0]), print_char);

	return 0;

}

void bsort(void *base, size_t nmemb, size_t size, 
						int (*compar)(const void *, const void *)){

	void *temp = malloc(size);

	
	for (int i = nmemb - 1; i > 0;i--)
	{
		for(int j = 0; j < i ; j++)
		{
			if (compar(base + j * size, base + (j + 1) * size ) > 0 )
			{
				{	
					memmove(temp, base + j * size, size );
					memmove(base + j * size, base + (j+1) * size , size);
					memmove(base + (j+1) * size, temp, size);
				}
			}

		}
	}

	free(temp);
}


int compar_int(const void *elem1, const void *elem2)
{
	int e1 = *(int *)elem1;
	int e2 = *(int *)elem2;

	if (e1 > e2) return 1;
	else return -1;
}

int compar_float(const void *elem1, const void *elem2)
{
	float e1 = *(int *)elem1;
	float e2 = *(int *)elem2;

	if (e1 > e2) return 1;
	else return -1;
}

int compar_char(const void *elem1, const void *elem2)
{
	char e1 = *(int *)elem1;
	char e2 = *(int *)elem2;

	if (e1 > e2) return 1;
	else return -1;
}

//사용자 함수int 출력
void print_int(void *elem) {
	printf("%d ", *(int *)elem);	//elem이 보이드 포인터인데 역참조해서 출력함
}

//사용자 함수 float 출력
void print_float(void *elem){
	printf("%.2f ", *(float *)elem);
}

void print_char(void *elem){
	printf("%c ", *(char *)elem);
}
//generic(범용적인) array 출력 함수
void generic_print_arr(void *base, size_t nmemb, size_t size,
										void (*print)(void*)) // 배열, 요소 수, 요소 별 크기, 적용할 함수
{
	for (int i = 0; i < nmemb; i++)
		print(base+i*size);// 얘는 왜 base + i * size 이냐면,,, 
	printf("\n");
}