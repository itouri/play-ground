#include "sgx_eid.h"
#include "error_codes.h"
#include "datatypes.h"
#include "sgx_urts.h"
#include "sgx_dh.h"
#include <string.h>

#include "../MasterEnclave/MasterEnclave_u.h"
#include "../LibEnclave/LibEnclave_u.h"

#include "error_codes.h"

#include "IO.h"

#include <map>

extern void do_main(void);

uint32_t load_enclave()
{
    uint32_t enclave_temp_no;
    int ret, launch_token_updated;
    sgx_launch_token_t launch_token;

    enclave_temp_no = 0;

    ret = sgx_create_enclave("emain.so", SGX_DEBUG_FLAG, &launch_token, &launch_token_updated, &app_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
                return ret;
    }

    enclave_temp_no++;
    g_enclave_id_map.insert(std::pair<sgx_enclave_id_t, uint32_t>(app_enclave_id, enclave_temp_no));

    return SGX_SUCCESS;
}

// to sgx_status_t
int la(const char * unix_sock_path) {
    //
    status = create_session(master_enclave_id, &ret_status, master_enclave_id, app_enclave_id);
    if (status!=SGX_SUCCESS) {
        printf("Enclave1_test_create_session Ecall failed: Error code is %x", status);
        return -1;
    }

    if (ret_status != 0) {
        printf("\nSession establishment and key exchange failure between Source (E1) and Destination (E2): Error code is %x", ret_status);
        return -1;
    }
    printf("\n\nSecure Channel Establishment between Source (E1) and Destination (E2) Enclaves successful !!!");

    return 0;
}

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        printf("absent unix_sock_path\n");
    }

    if(load_enclave() != SGX_SUCCESS)
    {
        printf("Load Enclave Failure\n");
    }

    status = create_session(master_enclave_id, &ret_status, master_enclave_id, app_enclave_id);
    if (status!=SGX_SUCCESS) {
        printf("create_session Ecall failed: Error code is %x", status);
        return -1;
    }

    if (ret_status != 0) {
        printf("\nSession establishment and key exchange failure: Error code is %x", ret_status);
        return -1;
    }
    printf("\n\nSecure Channel Establishment Enclaves successful !!!");
    return 0;    

    const char * unix_sock_path = argv[1];
    la(unix_sock_path);

    void (*func)(void) = do_main;
    (*func)();

    return 0;
}
