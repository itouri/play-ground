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

#define FILE_PATH "./unix_domain.uds"

void print_hex(uint8_t * data, size_t size) {
    int i;
    for (i=0; i < size; i++) {
        printf("%c", data[i]);
    }
    printf("\n");
}

// https://qiita.com/methane/items/a467a28c8359b045a498
int main()
{
    int r;
    int listen_fd = 0;
    struct sockaddr_un local, remote;

    uint8_t buf[1024];

    signal(SIGPIPE, SIG_IGN);
    listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, FILE_PATH);
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
            return 0;
        }
        sgx_enclave_id_t vm_enclave_id;
        uint8_t * create_req;

        uint8_t buf[1024]; //TODO サイズを決める
        memset(buf, 0, sizeof buf);

        int n;
        n = read(remote_fd, buf, sizeof buf);
        if (n < 0) {
            perror("read length is zero");
            return 1;
        }
        memcpy(&vm_enclave_id, buf, sizeof(sgx_enclave_id_t));

        int create_req_size = n - (sizeof(sgx_enclave_id_t));
        create_req = (uint8_t*)malloc(create_req_size);
        memcpy(create_req, (const void*)&buf[(sizeof(sgx_enclave_id_t))], create_req_size);

        //print_hex(buf, n);
        printf("sgx_enclave_id_t size: %ld\n create_req_size: %d\n", sizeof(sgx_enclave_id_t), create_req_size);
        printf("sgx_enclave_id: %ld\n", vm_enclave_id);

        print_hex((uint8_t*)&vm_enclave_id, sizeof(sgx_enclave_id_t));
        print_hex(create_req, create_req_size);
    }
    close(listen_fd); 
}