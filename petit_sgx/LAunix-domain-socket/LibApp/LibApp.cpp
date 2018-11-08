#include "sgx_eid.h"
#include "error_codes.h"
#include "datatypes.h"
#include "sgx_urts.h"
#include "sgx_dh.h"
#include <string.h>

#include "../MasterEnclave/MasterEnclave_u.h"
#include "../LibEnclave/LibEnclave_u.h"

#include "error_codes.h"

#include <map>

sgx_enclave_id_t vm_enclave_id = 0;

#define VM_ENC_PATH "./emain.so"

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


void send_session_request(sgx_dh_session_t * sgx_dh_session) {
    // send!
}

void read_responce(sgx_dh_msg1_t * msg1, sgx_dh_session_t * sgx_dh_session) {
    // read
}

//LIB
ATTESTATION_STATUS ocall_session_request(sgx_dh_session_t * sgx_dh_session)
{
	uint32_t status = 0;
	sgx_status_t ret = SGX_SUCCESS;

    sgx_dh_msg1_t * msg1;

    // send_session_request
    send_session_request(sgx_dh_session);
    read_responce(msg1, *sgx_dh_session);

    // 両方とも書き換える必要はないはず
    ecall_exchange_report(*msg1, *sgx_dh_session);

	// ret = session_request(dest_enclave_id, &status, src_enclave_id, dh_msg1, sgx_dh_session);
	// if (ret == SGX_SUCCESS)
	// 	return (ATTESTATION_STATUS)status;
	// else	
	//     return INVALID_SESSION;

}

//Makes an sgx_ecall to the destination enclave sends message2 from the source enclave and gets message 3 from the destination enclave
ATTESTATION_STATUS ocall_exchange_report(sgx_enclave_id_t src_enclave_id, sgx_enclave_id_t dest_enclave_id, sgx_dh_msg2_t *dh_msg2, sgx_dh_msg3_t *dh_msg3, uint32_t session_id)
{
	uint32_t status = 0;
	sgx_status_t ret = SGX_SUCCESS;


	ret = exchange_report(dest_enclave_id, &status, src_enclave_id, dh_msg2, dh_msg3, session_id);
	if (ret == SGX_SUCCESS)
		return (ATTESTATION_STATUS)status;
	else	
	    return INVALID_SESSION;

    return ret;
}



uint32_t load_vm_enclave()
{
    int ret, launch_token_updated;
    sgx_launch_token_t launch_token;

    ret = sgx_create_enclave(VM_ENC_PATH, SGX_DEBUG_FLAG, &launch_token, &launch_token_updated, &vm_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
                return ret;
    }

    return SGX_SUCCESS;
}

int main()
{
    uint32_t ret_status;
    sgx_status_t status;

    if(load_vm_enclave() != SGX_SUCCESS)
    {
        printf("\nLoad Enclave Failure");
    }

    status = ecall_create_session(vm_enclave_id), &ret_status, master_enclave_id, vm_enclave_id);
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