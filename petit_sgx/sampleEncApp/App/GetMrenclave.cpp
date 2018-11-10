#include "sgx_eid.h"
#include "sgx_urts.h"
#include <string.h>
#include <stdio.h>

#include "../LibEnclave/LibEnclave_u.h"

uint32_t load_app_enclave(const char * so_path, sgx_enclave_id_t * app_enclave_id)
{
    int ret, launch_token_updated;
    sgx_launch_token_t launch_token;

    ret = sgx_create_enclave(so_path, SGX_DEBUG_FLAG, &launch_token, &launch_token_updated, app_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
        return ret;
    }

    return SGX_SUCCESS;
}

int main(int argc, char const *argv[])
{
    if (argc != 0) {
        printf("argc is not 1\n");
    }

    sgx_enclave_id_t app_enclave_id;
    load_app_enclave(argv[1], &app_enclave_id);

    get_mrenclave(app_enclave_id);

    return 0;
}
