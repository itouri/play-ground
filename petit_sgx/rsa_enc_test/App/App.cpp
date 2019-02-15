#include <stdio.h>
#include <stdlib.h>

#include "sgx_eid.h"
#include "error_codes.h"
#include "datatypes.h"
#include "sgx_urts.h"
#include "sgx_dh.h"
#include <string.h>

#include "../MasterEnclave/MasterEnclave_u.h"

sgx_enclave_id_t master_enclave_id = 0;

#define MASTER_ENC_PATH "libMasterenclave.so"

void print_ocall(char *str)
{
	printf("%s", str);
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

void encrypt(unsigned char * enc_msg, unsigned char * msg, size_t msg_size, unsigned long pubkey, int n)
{
    int i, j;
    unsigned long k;

    for ( i=0; i < msg_size; i++ ) {
        k = 1;
        for (j=0; j < pubkey; j++) {
            k *= msg[i];
            k %= n;
        }
        enc_msg[i] = k;
    }

    printf("\nTHE ENCRYPTED MESSAGE IS\n");
    for (i = 0; i < msg_size; i++)
        printf("%02x ", enc_msg[i]);
    printf("\n");
}

int main()
{
    int i;
    int n;
    unsigned char msg[100];
    int msg_len;
    unsigned long pubkey;
    unsigned char * enc_msg;

    if(load_enclaves() != SGX_SUCCESS)
    {
        printf("\nLoad Enclave Failure");
        return -1;
    }

    printf("\nENTER MESSAGE OR STRING TO ENCRYPT\n");
    memset(msg, 0, sizeof(msg));
    scanf("%s", msg);

    printf("\nINPUTED MESSAGE\n");
    for (i = 0; i < strlen((const char*)msg); i++)
        printf("%02x ", msg[i]);
    printf("\n");

    msg_len = strlen((const char *)msg);
    enc_msg = (unsigned char*)malloc(msg_len);

    n = 253;
    pubkey = 3;
    //prvkey = 147;

    encrypt(enc_msg, msg, msg_len, pubkey, n);

    int ret = test_ecall(master_enclave_id, enc_msg, msg_len, n);
    if (ret != SGX_SUCCESS) {
        printf("failed: %x\n", ret);
    }
    return 0;
}
