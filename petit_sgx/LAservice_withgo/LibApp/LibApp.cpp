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

#define LA_SOCK_PATH "/tmp/compute/socket/"

#define VM_ENC_PATH "./libVMenclave.so"

sgx_enclave_id_t vm_enclave_id = 0;

extern void do_main(sgx_enclave_id_t);
extern void get_mrenclave(sgx_enclave_id_t);

void print_hex(uint8_t * str, size_t size) {
    int i;
    for (i=0; i<size; i++) {
        printf("%02x", str[i]);
    }
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
        fprintf(stderr, "lib_app: socket error errno[%d]\n", errno);
        exit(-1);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    //strcpy(addr.sun_path, unix_domain_path);
    strcpy(addr.sun_path, unix_domain_path);

    if(connect(client_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0){
        fprintf(stderr, "lib_app: connect error errno[%d]\n", errno);
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

sgx_status_t load_vm_enclave()
{
    sgx_status_t ret;
    int launch_token_updated;
    sgx_launch_token_t launch_token;

    ret = sgx_create_enclave(VM_ENC_PATH, SGX_DEBUG_FLAG, &launch_token, &launch_token_updated, &vm_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
        return ret;
    }

    return SGX_SUCCESS;
}

int main(int argc, char *argv[])
//int la(sgx_enclave_id_t enclave_id)
{
    uint32_t ret_status;
    sgx_status_t status;

    status = load_vm_enclave();
    if (status!=SGX_SUCCESS) {
        printf("lib_app: load_vm_enclave failed: Error code is %x\n", status);
        //sgx_destroy_enclave(enclave_id);
        return -1;
    }

    // get mrenclve
    if ( argc == 1 ) {
        void (*func)(sgx_enclave_id_t) = get_mrenclave;
        (*func)(vm_enclave_id);
        return 0;
    }

    char * socket_path = argv[1];
    printf("lib_app: vm_app socket path: %s\n", socket_path);

    init_client(socket_path);

    /* calling main */
    void (*func)(sgx_enclave_id_t) = do_main;
    (*func)(vm_enclave_id);
    //do_main(vm_enclave_id);

    // sgx_destroy_enclave(vm_enclave_id);
    return 0;
}