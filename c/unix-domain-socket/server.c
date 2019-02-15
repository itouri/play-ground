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

#define FILE_PATH "./unix_domain.uds"

void print_hex(uint8_t * data, size_t size) {
    int i;
    for (i=0; i < size; i++) {
        printf("%x ", data[i]);
    }
    printf("\n");
}

typedef uint8_t image_id_t[16];

/* データ構造
image_id_t(uint8[16]) image_id
uint32_t image_metadata_size
uint32_t create_req_metadata_size
uint8_t * image_metadata
uint8_t * create_req_metadata
*/

void serve(int remote_fd) {
    image_id_t image_id;
    uint8_t * image_metadata;
    uint8_t * create_req_metadata;

    uint8_t buf[1024]; //TODO サイズを決める
    memset(buf, 0, sizeof buf);

    int n;
    n = read(remote_fd, buf, sizeof buf);
    if (n < 0) {
        perror("read length is zero");
        return;
    }
    memcpy(&image_id, buf, sizeof(image_id_t));

    size_t offset = (sizeof(image_id_t));

    uint32_t image_metadata_size;
    uint32_t create_req_metadata_size;

    memcpy(&image_metadata_size, &buf[offset], sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&create_req_metadata_size, &buf[offset], sizeof(uint32_t));
    offset += sizeof(uint32_t);

    image_metadata = (uint8_t*)malloc(image_metadata_size);
    memcpy(image_metadata, (const void*)&buf[offset], image_metadata_size);

    offset += image_metadata_size;

    create_req_metadata = (uint8_t*)malloc(create_req_metadata_size);
    memcpy(create_req_metadata, (const void*)&buf[offset], create_req_metadata_size);

    //print_hex(buf, n);
    printf("image_id_t size: %ld\n", sizeof(image_id_t));
    printf("image_metadata_size: %d\n", image_metadata_size);
    printf("create_req_metadata_size: %d\n", create_req_metadata_size);

    //printf("sgx_enclave_id: %s\n", image_id);

    print_hex((uint8_t*)&image_id, sizeof(image_id_t));
    print_hex(image_metadata, image_metadata_size);
    print_hex(create_req_metadata, create_req_metadata_size);
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
        serve(remote_fd);
        close(remote_fd);
    }
    close(listen_fd); 
}