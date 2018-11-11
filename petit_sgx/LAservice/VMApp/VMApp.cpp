#include "../VMEnclave/VMEnclave_u.h"

void ocall_print(char *str)
{
    printf("%s", str);
}

void do_main(sgx_enclave_id_t enclave_id)
{
    printf("hello world from untrust!\n");
    ecall_test(enclave_id);
    return;
}
