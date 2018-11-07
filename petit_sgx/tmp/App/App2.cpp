#include "sgx_eid.h"
#include "error_codes.h"
#include "datatypes.h"
#include "sgx_urts.h"
#include "sgx_dh.h"
#include <string.h>

#include "../MasterEnclave/MasterEnclave_u.h"

#include <map>

sgx_enclave_id_t master_enclave_id = 0;

#define MASTER_ENC_PATH "libMasterenclave.so"

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

uint32_t load_enclaves()
{
    uint32_t enclave_temp_no;
    int ret, launch_token_updated;
    sgx_launch_token_t launch_token;

    ret = sgx_create_enclave(MASTER_ENC_PATH, SGX_DEBUG_FLAG, &launch_token, &launch_token_updated, &master_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
        return ret;
    }
    return SGX_SUCCESS;
}

int main()
{
    if(load_enclaves() != SGX_SUCCESS)
    {
        printf("\nLoad Enclave Failure");
        return -1;
    }
    printf("id: %u\n", (uint64_t)master_enclave_id);

    int ret = test_ecall(master_enclave_id, master_enclave_id);
    if (ret != SGX_SUCCESS) {
        printf("failed: %x\n", ret);
    }

    getchar();
    return 0;
}
