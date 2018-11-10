#include <string.h>
#include <stdio.h>

void ocall_print_string(char *str)
{
	int i;
	int size = strlen((const char*)str) >= 32 ? 32 : strlen((const char*)str);
	printf("\n");
	for(i = 0; i < size; ++i) {
		printf("%x ",str[i]);
	}
	printf("\n");
	for(i = 0; i < size; ++i) {
		printf("%c ",str[i]);
	}
	printf("size: %d\n", strlen((const char*)str));
	fflush(stdout);
}
