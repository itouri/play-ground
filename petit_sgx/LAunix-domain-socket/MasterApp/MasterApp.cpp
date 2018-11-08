#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <signal.h>

#include "sgx_eid.h"
#include "sgx_urts.h"
#include "sgx_dh.h"

#include "../MasterEnclave/MasterEnclave_u.h"

#define GOLANG_SOCK_PATH "golang.uds"
#define LA_SOCK_PATH "la.uds"

#define MASTER_ENC_PATH "libMasterenclave.so"

sgx_enclave_id_t master_enclave_id;

void ocall_print(char *str)
{
	// int i;
	// int size = strlen((const char*)str) >= 32 ? 32 : strlen((const char*)str);
	// printf("\n");
	// for(i = 0; i < size; ++i) {
	// 	printf("%x ",str[i]);
	// }
	// printf("\n");
	// for(i = 0; i < size; ++i) {
	// 	printf("%c ",str[i]);
	// }
	// printf("size: %d\n", strlen((const char*)str));
	// fflush(stdout);
    printf("%s", str);
}

void read_session_request(int remote_fd) {
    sgx_dh_msg1_t dh_msg1;
    uint8_t buf[1024]; //TODO サイズを決める
    uint32_t ret_val;

    // read 今回送るデータないけど
    if(read(remote_fd, buf, strlen("a")) < 0){
        fprintf(stderr, "read session req error errno[%d]\n", errno);
        exit(-1);
    }

    ecall_session_request(master_enclave_id, &ret_val, &dh_msg1);

    // send
    if(write(remote_fd, &dh_msg1, sizeof(dh_msg1)) < 0){
        fprintf(stderr, "write session req error errno[%d]\n", errno);
        exit(-1);
    }    
}

void read_exchange_report (int remote_fd) {
    sgx_dh_msg2_t dh_msg2;
    sgx_dh_msg3_t dh_msg3;
    uint32_t ret_val;

    if(read(remote_fd, &dh_msg2, sizeof(dh_msg2)) < 0){
        fprintf(stderr, "read exchange_report req error errno[%d]\n", errno);
        exit(-1);
    }

    ecall_exchange_report(master_enclave_id, &ret_val, dh_msg2, &dh_msg3);

    // send
    if(write(remote_fd, &dh_msg3, sizeof(dh_msg3)) < 0){
        fprintf(stderr, "write exchange_report req error errno[%d]\n", errno);
        exit(-1);
    }
}


void serve (int remote_fd) {
    read_session_request(remote_fd);
    read_exchange_report(remote_fd);
}

void run_server () {
    int r;
    int listen_fd = 0;
    struct sockaddr_un local, remote;

    signal(SIGPIPE, SIG_IGN);
    listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, LA_SOCK_PATH);
    unlink(local.sun_path);
    r = bind(listen_fd, (struct sockaddr *)&local, sizeof(local));
    if (r)
        perror("failed to bind");

    listen(listen_fd, 100);
    for (;;) {
        socklen_t len = sizeof(remote);
        int remote_fd = accept(listen_fd, (struct sockaddr *)&remote, &len);
        if (remote_fd < 0) {
            perror("failed to accept");
            return;
        }
        serve(remote_fd);
        close(remote_fd);
    }
    close(listen_fd); 
}

uint32_t load_master_enclave()
{
    int ret, launch_token_updated;
    sgx_launch_token_t launch_token;

    ret = sgx_create_enclave(MASTER_ENC_PATH, SGX_DEBUG_FLAG, &launch_token, &launch_token_updated, &master_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
                return ret;
    }

    return SGX_SUCCESS;
}

int main()
{
    // load_enclave()
    if(load_master_enclave() != SGX_SUCCESS)
    {
        printf("\nLoad Enclave Failure");
    }

    // server 開始
    run_server();
}