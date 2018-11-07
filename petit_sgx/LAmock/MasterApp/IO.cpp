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

#include <uuid/uuid.h>

#define FILE_PATH "./unix_domain.uds"

typedef uint8_t image_id_t[16];
const int uuid_len = 36+1;

void print_hex(uint8_t * data, size_t size) {
    int i;
    for (i=0; i < size; i++) {
        printf("%x ", data[i]);
    }
    printf("\n");
}

void serve(int remote_fd) {
    //
    
}

void la_serve(const char * socket_path) {
    int r;
    int listen_fd = 0;
    struct sockaddr_un local, remote;

    uint8_t buf[1024];

    signal(SIGPIPE, SIG_IGN);
    listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, socket_path);
    unlink(local.sun_path);
    r = bind(listen_fd, (struct sockaddr *)&local, sizeof(local));
    if (r)
        perror("failed to bind");

    listen(listen_fd, 101); // 2つ目の引数は何？
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

void run_la_server(image_id_t image_id, ) {
    /* uuid を持った socket のパスを生成 */
    uuid_t socket_uuid;
    char socket_name[uuid_len];
    uuid_generate(socket_uuid);
    uuid_unparse(socket_uuid, socket_name);

    const char * socket_path = SOCK_PATH + socket_name;

    // 実行する app のパス
    const char * app_path = FILE_PATH + image_id;
    system(./app_path, socket_path);

    la_serve(socket_path);
}

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
    
    // create run_la_server thread
    run_la_server();

    print_hex((uint8_t*)&image_id, sizeof(image_id_t));
    print_hex(image_metadata, image_metadata_size);
    print_hex(create_req_metadata, create_req_metadata_size);
}

void run_server() {
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
            return;
        }
        serve(remote_fd);
        close(remote_fd);
    }
    close(listen_fd); 
}