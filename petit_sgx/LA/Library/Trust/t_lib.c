#include "sgx_eid.h"
#include "error_codes.h"
#include "sgx_dh.h"

#include "sgx_utils.h"

void print_hex(uint8_t * str, size_t size) {
    int i;
    for (i=0; i<size; i++) {
        printe("%x", str[i]);
    }
}

void ecall_get_mrenclave()
{
    sgx_report_t report;
    sgx_status_t ret;

    //printe("begin sgx_create_report\n");
    ret = sgx_create_report(NULL, NULL, &report);
    if ( ret != SGX_SUCCESS )
    {
        printe("failed sgx_create_report\n");
        return ret;
    }

    // ret = sgx_verify_report(&report);
    // if ( ret != SGX_SUCCESS )
    // {
    //     printe("failed sgx_verify_report\n");
    //     return ret;
    // }

    printe((char*)&report.body.mr_enclave);
}