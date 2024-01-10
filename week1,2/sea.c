#include <stdio.h>
#include <stdlib.h>

int global1;
int global2;
int global3;
int global4;

void func(void)
{
	char local5;
	char local6;
	
	printf("%p\taddress of local5\n", &local5);
	printf("%p\taddress of local6\n", &local6);

}

void func2(void)
{
	char local7;
	char local8;
	
	printf("%p\taddress of local7\n", &local7);
	printf("%p\taddress of local8\n", &local8);

}


int main(void)
{

	static char static1;
	static char static2 = 's';
	static char static3;
	static char static4 = 'S';
	
	char local1;
	char local2 = 'l';
	char local3;
	char local4 = 'L';
	
	char *ptr;
	
	ptr = (char *)malloc(100 * sizeof(char));
	
	
	
	printf("%p\taddress of global1\n", &global1);
	printf("%p\taddress of global2\n", &global2);
	printf("%p\taddress of global3\n", &global3);
	printf("%p\taddress of global4\n", &global4);
	
	printf("%p\taddress of static1\n", &static1);
	printf("%p\taddress of static2\n", &static2);
	printf("%p\taddress of static3\n", &static3);
	printf("%p\taddress of static4\n", &static4);
	
	printf("%p\taddress of local1\n", &local1);
	printf("%p\taddress of local2\n", &local2);
	printf("%p\taddress of local3\n", &local3);
	printf("%p\taddress of local4\n", &local4);
	
	printf("%p\tvalue of ptr\n", ptr);
	
	printf("%p\taddress of main\n", main);
	printf("%p\taddress of func\n", func);
	printf("%p\taddress of func2\n", &func2); /*doesn't matter that & being or not*/

	free(ptr);
	
	func();
	func2();
	return 0;
}
