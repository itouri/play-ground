#include<stdio.h>

#include "../VMEnclave/VMEnclave_u.h"

void print_ocall(char *str)
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

void do_main(){
    printf("hello world from untrust!!\n");
    test_ecall(sgx_enclave_id_t enclave_id)
}