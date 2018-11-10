#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "measurement_t.h"
#include "measurement.h"

#include "error_codes.h"

#include "sgx_eid.h"
#include "sgx_urts.h"

extern void do_main(sgx_enclave_id_t);

ATTESTATION_STATUS session_request_ocall(sgx_enclave_id_t src_enclave_id, sgx_enclave_id_t dest_enclave_id, sgx_dh_msg1_t* dh_msg1, uint32_t* session_id)
{
	uint32_t status = 0;
	sgx_status_t ret = SGX_SUCCESS;

	ret = session_request_ecall(dest_enclave_id, &status, src_enclave_id, dh_msg1, session_id);
	if (ret == SGX_SUCCESS)
		return (ATTESTATION_STATUS)status;
	else	
	    return INVALID_SESSION;
}



sgx_enclave_id_t init_app(sgx_enclave_id_t * master_enclave_id) {
    const char * enclave_so_path = "emain.so"
    
    int ret, launch_token_updated;
    sgx_launch_token_t launch_token;
    sgx_status_t ret;
    sgx_enclave_id_t enclave_id;

    ret = sgx_create_enclave(enclave_so_path, SGX_DEBUG_FLAG, &launch_token, &launch_token_updated, enclave_id, NULL);
    if ( ret != SGX_SUCESS ) {
        perror("failed sgx_create_enclave: %x\n" ret);
    }
<<<<<<< HEAD

    

=======
>>>>>>> 726c3bff0bf977c8b396da028e8ce3f28358b3fb
    return enclave_id;
}

void main (int argc, char * argv[]) {
    if ( argc != 1 ) {
        // このライブラリで定義した main を書いてもらうしかない
        perror("absent mr and master_enclave_id");
        return 0;
    }

    if ( strcmp("mr", argv[1]) == 0 ) {
        ecall_get_mrenclave();
        return 0;
    }

    uint64_t master_enclave_id;
    master_enclave_id = (uint64_t)atoll(argv[1]);

    sgx_enclave_id_t enclave_id;
    enclave_id = init_app(master_enclave_id);

    // TODO コマンドライン引数とかを取れるようにしないと
    do_main(enclave_id);
}