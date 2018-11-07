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

#define GOLANG_SOCK_PATH "golang.uds"
#define LA_SOCK_PATH "la.uds"

#define MASTER_ENC_PATH "libMasterenclave.so"

sgx_enclave_id_t master_enclave_id;

void read_session_request(int remote_fd, sgx_dh_msg1_t * dh_msg1, sgx_dh_session_t * sgx_dh_session) {
    session_request();
}

void send_responce (int remote_fd, sgx_dh_msg1_t dh_msg1, sgx_dh_session_t sgx_dh_session) {
    // 送る情報は sgx_dh_msg1_t と sgx_dh_session

}

void read_exchange_report (int remote_fd, sgx_dh_msg3_t * dh_msg3) {
    exchange_report();
}

void send_msg3(int remote_fd, sgx_dh_msg3_t dh_msg3) {
    
}

void ocall_send_responce() {

}

void serve (int remote_fd) {
    sgx_dh_msg1_t *dh_msg1;
    sgx_dh_session_t *sgx_dh_session;
    sgx_dh_msg3_t *dh_msg3;

    read_session_request(remote_fd, dh_msg1, sgx_dh_session);
    send_responce(remote_fd, *dh_msg1, *sgx_dh_session);
    read_exchange_report(remote_fd, dh_msg3);
    send_msg3(remote_fd, *dh_msg3);
}

void run_server () {
    int r;
    int listen_fd = 0;
    struct sockaddr_un local, remote;

    uint8_t buf[1024];

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
    uint32_t enclave_temp_no;
    int ret, launch_token_updated;
    sgx_launch_token_t launch_token;

    enclave_temp_no = 0;

    ret = sgx_create_enclave(MASTER_ENC_PATH, SGX_DEBUG_FLAG, &launch_token, &launch_token_updated, &master_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
                return ret;
    }

    enclave_temp_no++;
    g_enclave_id_map.insert(std::pair<sgx_enclave_id_t, uint32_t>(app_enclave_id, enclave_temp_no));

    return SGX_SUCCESS;
}

int main()
{
    sgx_enclave_id_t master_enclave_id;
    // load_enclave()
    if(load_master_enclave() != SGX_SUCCESS)
    {
        printf("\nLoad Enclave Failure");
    }

    // server 開始
    run_server();
}