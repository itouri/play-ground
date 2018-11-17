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

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t pid;
void ocall_print_vm(char *str)
{
    printf("[%05d]: %s", pid, str);
}

void init (sgx_enclave_id_t enclave_id)
{
    uint32_t ret_status;
    sgx_status_t status;

    status = ecall_init(enclave_id, &ret_status);
    if (status!=SGX_SUCCESS) {
        printf("[%05d]: Enclave1_test_create_session Ecall failed: Error code is %x\n", pid, status);
        //sgx_destroy_enclave(enclave_id);
        return;
    }

    if (ret_status != 0) {
        printf("[%05d]: Session establishment and key exchange failure: Error code is %x\n", pid, ret_status);
        //sgx_destroy_enclave(enclave_id);
        return;
    }
    printf("[%05d]: Secure Channel Establishment Enclaves successful !!!\n", pid);
}

void get_mrenclave(sgx_enclave_id_t enclave_id) {
    ecall_get_mr_enclave(enclave_id);
}

void do_main(sgx_enclave_id_t enclave_id)
//int main()
{
    pid_t pid = getpid();
    uint32_t ret_status;
    sgx_status_t status;

    init(enclave_id);

    status = ecall_test(enclave_id);
    if (status!=SGX_SUCCESS) {
        printf("[%05d]: ecall_test failed: Error code is %x\n", pid, status);
        exit(-1);
    }

    printf("[%05d]: hello world from untrust! : %x\n", pid, enclave_id);
}
