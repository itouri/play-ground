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

#include "sgx_eid.h"
#include "sgx_urts.h"
#include "sgx_dh.h"

#include "datatypes.h"

#include "pthread.h"

#include "../MasterEnclave/MasterEnclave_u.h"

#define IMAGE_PATH "/tmp/compute/image/"

#define GOLANG_SOCK_PATH "/tmp/compute/golang.uds"
#define LA_SOCK_PATH "/tmp/compute/socket/"

#define MASTER_ENC_PATH "libMasterenclave.so"

sgx_enclave_id_t master_enclave_id;

void print_hex(uint8_t * str, size_t size) {
    int i;
    for (i=0; i<size; i++) {
        printf("%d ", str[i]);
    }
    printf("\n");
}

void ocall_print(char *str)
{
    printf("%s", str);
}

void read_session_request(int remote_fd) {
    sgx_dh_msg1_t dh_msg1;
    uint8_t buf[2]; //TODO サイズを決める
    uint32_t ret_val;
    sgx_status_t status;

    memset(&dh_msg1, 0, sizeof(sgx_dh_msg1_t));

    // read 今回送るデータないけど
    if(read(remote_fd, buf, strlen("a")) < 0){
        fprintf(stderr, "read session req error errno[%d]\n", errno);
        exit(-1);
    }

    status = ecall_session_request(master_enclave_id, &ret_val, &dh_msg1);
    if (status!=SGX_SUCCESS) {
        printf("ecall_session_request failed: Error code is %x\n", status);
        exit(-1);
    }
    if (ret_val != 0) {
        printf("session_request failure: Error code is %x\n", ret_val);
        exit(-1);
    }
    // send
    if(write(remote_fd, &dh_msg1, sizeof(sgx_dh_msg1_t)) < 0){
        fprintf(stderr, "write session req error errno[%d]\n", errno);
        exit(-1);
    }
    //print_hex((uint8_t*)&dh_msg1, sizeof(sgx_dh_msg1_t));
}

void read_exchange_report (int remote_fd, la_server_arg_t la) {
    sgx_dh_msg2_t dh_msg2;
    sgx_dh_msg3_t dh_msg3;
    uint32_t ret_val;
    sgx_status_t status;

    if( read(remote_fd, &dh_msg2, sizeof(sgx_dh_msg2_t)) < 0 ){
        fprintf(stderr, "read exchange_report req error errno[%d]\n", errno);
        exit(-1);
    }

    status = ecall_exchange_report(master_enclave_id, &ret_val, dh_msg2, &dh_msg3, la.arg);
        if (status!=SGX_SUCCESS) {
        printf("ecall_exchange_report failed: Error code is %x\n", status);
        sgx_destroy_enclave(master_enclave_id);
        exit(-1);
    }
    if (ret_val != 0) {
        printf("exchange_report failure?: Error code is %x\n", ret_val);
        exit(-1);
    }

    // send
    if( write(remote_fd, &dh_msg3, sizeof(sgx_dh_msg3_t)) < 0 ){
        fprintf(stderr, "write exchange_report req error errno[%d]\n", errno);
        exit(-1);
    }
}

void serve (int remote_fd, la_server_arg_t la_md) {
    read_session_request(remote_fd);
    read_exchange_report(remote_fd, la_md);
    printf("\nok!\n");
}

void *run_la_server (void * void_arg) {
    int r;
    int listen_fd = 0;
    struct sockaddr_un local, remote;
    la_server_arg_t * arg = (la_server_arg_t*)void_arg;
    //la_arg_t la_arg;

    signal(SIGPIPE, SIG_IGN);
    listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, (const char *)arg->socket_path); //(const char *)arg->socket_path);
    unlink(local.sun_path);
    r = bind(listen_fd, (struct sockaddr *)&local, sizeof(local));
    if (r)
        perror("la: failed to bind");

    listen(listen_fd, 100);
    printf("la: server start at %s\n", arg->socket_path);//TODO
    for (;;) {
        socklen_t len = sizeof(remote);
        int remote_fd = accept(listen_fd, (struct sockaddr *)&remote, &len);
        if (remote_fd < 0) {
            perror("la: failed to accept");
            return 0;
        }
        serve(remote_fd, *arg);
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

void launch_vm (image_id_t image_id, la_arg_t arg)
{
    uuid_t socket_uuid;
    char socket_id[37];
    uuid_generate(socket_uuid);
    uuid_unparse(socket_uuid, socket_id);

    char socket_path[256] = LA_SOCK_PATH;
    //strcat(socket_path, LA_SOCK_PATH);
    strcat(socket_path, (const char*)&socket_id);
    strcat(socket_path, ".uds"); //(const char*)image_id); //TODO!
    printf("vm_cpp socket_path: %s\n", socket_path);

    la_server_arg_t * th_arg = (la_server_arg_t*)malloc(sizeof(la_server_arg_t));
    th_arg->socket_path = (unsigned char*)socket_path;
    th_arg->arg = arg;

    pthread_t pthread;
    pthread_create(&pthread, NULL, &run_la_server, (void*)th_arg);

    //char vm_app_path[256] = IMAGE_PATH;
    char vm_app_path[256] = ".";
    //strcat(vm_app_path, (const char*)image_id);
    strcat(vm_app_path, "/vm_app");
    printf("vm_app_path: %s\n", vm_app_path);

    char cmd[512] = "";
    strcat(cmd, vm_app_path);
    strcat(cmd, " ");
    strcat(cmd, socket_path);
    printf("command: %s\n", cmd);

    system(cmd);

    //pthread_join(pthread, NULL);
}

void go_serve (int remote_fd) {
    image_id_t image_id;
    uint8_t * image_metadata;
    uint8_t * create_req_metadata;
    la_arg_t la_arg;
    memset(&la_arg, 0, sizeof(la_arg_t));

    uint8_t buf[1024]; //TODO サイズを決める
    memset(buf, 0, sizeof buf);

    int n;
    n = read(remote_fd, buf, sizeof buf);
    if (n < 0) {
        perror(
            "read length is zero");
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

    la_arg.imd = image_metadata;
    la_arg.imd_sz = image_metadata_size;
    la_arg.crm = create_req_metadata;
    la_arg.crm_sz = create_req_metadata_size; 

    launch_vm(image_id, la_arg);

    //print_hex(buf, n);
    printf("image_id_t size: %ld\n", sizeof(image_id_t));
    printf("image_metadata_size: %d\n", image_metadata_size);
    printf("create_req_metadata_size: %d\n", create_req_metadata_size);
    
    print_hex((uint8_t*)&image_id, sizeof(image_id_t));
    print_hex(image_metadata, image_metadata_size);
    print_hex(create_req_metadata, create_req_metadata_size);

    printf("--- la_arg ---\n");
    print_hex(la_arg.imd, la_arg.imd_sz);
    print_hex(la_arg.crm, la_arg.crm_sz);
}

void run_go_server ()
{
    int r;
    int listen_fd = 0;
    struct sockaddr_un local, remote;

    signal(SIGPIPE, SIG_IGN);
    listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, GOLANG_SOCK_PATH);
    unlink(local.sun_path);
    r = bind(listen_fd, (struct sockaddr *)&local, sizeof(local));
    if (r)
        printf("failed to bind: %s\n", GOLANG_SOCK_PATH);

    listen(listen_fd, 100);
    for (;;) {
        socklen_t len = sizeof(remote);
        int remote_fd = accept(listen_fd, (struct sockaddr *)&remote, &len);
        if (remote_fd < 0) {
            perror("failed to accept");
            return;
        }
        go_serve(remote_fd);
        close(remote_fd);
    }
    close(listen_fd); 
}

int main()
{
    // load_enclave()
    if(load_master_enclave() != SGX_SUCCESS)
    {
        printf("\nLoad Enclave Failure");
    }

    run_go_server();
}