#include <stdio.h>

//generic 하지 않은, 그냥 출력 함수
void print_arr(int *base, int size)
{
	for(int i = 0; i < size; i++)
		printf("%d ", *(base+i)); // pointer + 연산
	printf("\n");
}


//사용자 함수int 출력
void print_int(void *elem) {
	printf("%d ", *(int *)elem);	//elem이 보이드 포인터인데 역참조해서 출력함
}

//사용자 함수 float 출력
void print_float(void *elem){
	printf("%.2f ", *(float *)elem);
}
//generic(범용적인) array 출력 함수
void generic_print_arr(void *base, size_t nmemb, size_t size,void (*print)(void*)) // 배열, 요소 수, 요소 별 크기, 적용할 함수
{
	for (int i = 0; i < nmemb; i++)
		print(base+i*size);// 얘는 왜 base + i * size 이냐면,,, 
	printf("\n");
}

//main 함수
int main(void) {

	int arr[10] = {3, 53, 2, 42, 66, 23, 63, 4, 111};
	float arr_f[10] = {3.4, 2.3, 36.3, 12.3, 34.2, 52.2, 53.1, 23.12, 32.2, 32.3};

	print_arr(arr,10);

	generic_print_arr(arr, 10, sizeof(int), print_int);
	generic_print_arr(arr_f, 10, sizeof(float), print_float);


	return 0;

}
