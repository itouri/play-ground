#include "sgx_eid.h"
#include "error_codes.h"
#include "datatypes.h"
#include "sgx_urts.h"
#include "sgx_dh.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../MasterEnclave/MasterEnclave_u.h"

#include <map>

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

int main(size_t argc, char *argv[])
{
    sgx_status_t ret = SGX_SUCCESS;
    sgx_enclave_id_t master_enclave_id;
    memcpy(&master_enclave_id, argv[1], sizeof(uint64_t));
    master_enclave_id = (uint64_t)atoi(argv[1]);
    printf("%d\n", master_enclave_id);
    
    ret = test_ecall(master_enclave_id, master_enclave_id);
    if (ret != SGX_SUCCESS) {
        printf("failed: %x\n", ret);
    }

    getchar();
    return 0;
}