#include<stdio.h>

#include "../VMEnclave/VMEnclave_u.h"
#include "../LibApp/LibApp.h"

#include "sgx_eid.h"
#include "error_codes.h"
#include "sgx_urts.h"
#include "sgx_dh.h"
#include <string.h>

#include "sgx_eid.h"
#include "error_codes.h"

void ocall_print_vm(char *str)
{
    printf("%s", str);
}

void init (sgx_enclave_id_t enclave_id)
{
    uint32_t ret_status;
    sgx_status_t status;

    status = ecall_init(enclave_id, &ret_status);
    if (status!=SGX_SUCCESS) {
        printf("Enclave1_test_create_session Ecall failed: Error code is %x\n", status);
        //sgx_destroy_enclave(enclave_id);
        return;
    }

    if (ret_status != 0) {
        printf("Session establishment and key exchange failure: Error code is %x\n", ret_status);
        //sgx_destroy_enclave(enclave_id);
        return;
    }
    printf("Secure Channel Establishment Enclaves successful !!!\n");
}

void do_main(sgx_enclave_id_t enclave_id)
//int main()
{
    uint32_t ret_status;
    sgx_status_t status;

    init(enclave_id);

    status = ecall_test(enclave_id);
    if (status!=SGX_SUCCESS) {
        printf("ecall_test failed: Error code is %x\n", status);
        exit(-1);
    }

    printf("hello world from untrust! : %x\n", enclave_id);
}
