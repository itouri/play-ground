#include "sgx_eid.h"
#include "error_codes.h"
#include "sgx_urts.h"
#include "sgx_dh.h"
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <signal.h>

#include "../LibEnclave/LibEnclave_u.h"

#include "error_codes.h"

#include <map>

sgx_enclave_id_t vm_enclave_id = 0;

#define VM_ENC_PATH "./libLibenclave.so"

void print_hex(uint8_t * str, size_t size) {
    int i;
    for (i=0; i<size; i++) {
        printf("%x", str[i]);
    }
    printf("\n");
}

void ocall_print(char *str)
{
    printf("%s", str);
}

struct sockaddr_un addr;
int client_fd;

void init_client(const char * unix_domain_path) {
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(client_fd < 0){
        fprintf(stderr, "socket error errno[%d]\n", errno);
        exit(-1);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, unix_domain_path);

    if(connect(client_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0){
        fprintf(stderr, "connect error errno[%d]\n", errno);
        exit(-1);
    }
}

ATTESTATION_STATUS ocall_session_request(sgx_dh_msg1_t * dh_msg1) {
    // send
    if(write(client_fd, "a", strlen("a")) < 0){
        fprintf(stderr, "write session req error errno[%d]\n", errno);
        return UNIX_DOMAIN_SOCKET_EEROR;
    }

    // read
    if( read(client_fd, dh_msg1, sizeof(sgx_dh_msg1_t)) < 0 ){
        fprintf(stderr, "read session req error errno[%d]\n", errno);
        return UNIX_DOMAIN_SOCKET_EEROR;
    }
    return SUCCESS;
}

ATTESTATION_STATUS ocall_exchange_report (sgx_dh_msg2_t dh_msg2, sgx_dh_msg3_t * dh_msg3)
{
    // send
    if( write(client_fd, &dh_msg2, sizeof(sgx_dh_msg2_t)) < 0 ){
        fprintf(stderr, "write session req error errno[%d]\n", errno);
        return UNIX_DOMAIN_SOCKET_EEROR;
    }

    // read
    if( read(client_fd, dh_msg3, sizeof(sgx_dh_msg3_t)) < 0 ){
        fprintf(stderr, "read session req error errno[%d]\n", errno);
        return UNIX_DOMAIN_SOCKET_EEROR;
    }
    return SUCCESS;
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
        printf("Load Enclave Failure\n");
    }

    init_client("la.uds");

    printf("id: %ld\n", (uint64_t)vm_enclave_id);

    status = ecall_create_session(vm_enclave_id, &ret_status);
    if (status!=SGX_SUCCESS) {
        printf("Enclave1_test_create_session Ecall failed: Error code is %x\n", status);
        sgx_destroy_enclave(vm_enclave_id);
        return -1;
    }

    if (ret_status != 0) {
        printf("Session establishment and key exchange failure: Error code is %x\n", ret_status);
        sgx_destroy_enclave(vm_enclave_id);
        return -1;
    }
    sgx_destroy_enclave(vm_enclave_id);
    printf("Secure Channel Establishment Enclaves successful !!!\n");
    return 0;
}