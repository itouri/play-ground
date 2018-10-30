/*
 * Copyright (C) 2011-2018 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


// App.cpp : Defines the entry point for the console application.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include "../MasterEnclave/MasterEnclave_u.h"
#include "../GrapheneEnclave/GrapheneEnclave_u.h"
#include "sgx_eid.h"
#include "sgx_urts.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <signal.h>

#define UNUSED(val) (void)(val)
#define TCHAR   char
#define _TCHAR  char
#define _T(str) str
#define scanf_s scanf
#define _tmain  main

#define IMAGE_ID_SIZE 8

extern std::map<sgx_enclave_id_t, uint32_t>g_enclave_id_map;


sgx_enclave_id_t master_enclave_id = 0;
sgx_enclave_id_t graphene_enclave_id = 0;

#define MASTER_ENC_PATH "libMasterenclave.so"
#define GRAPHENE_ENC_PATH "libGrapheneenclave.so"

void waitForKeyPress()
{
    char ch;
    int temp;
    printf("\n\nHit a key....\n");
    temp = scanf_s("%c", &ch);
    (void) temp;
}

sgx_status_t create_enclave(const char * image_id, sgx_enclave_id_t * dest_enclave_id) {
    int ret, launch_token_updated;
    sgx_launch_token_t launch_token;
    sgx_status_t ret;

    // こういうことがしたい
    enclave_file_path += IMG_PATH + image_id + "/enclave.so";

    ret = sgx_create_enclave(enclave_file_path, SGX_DEBUG_FLAG, &launch_token, &launch_token_updated, dest_enclave_id, NULL);
    return ret;
}

// 
void run_request_server() {
    // https://qiita.com/methane/items/a467a28c8359b045a498
    int r;
    int listen_fd = 0;
    struct sockaddr_un local, remote;

    signal(SIGPIPE, SIG_IGN);
    listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, "test.uds");
    unlink(local.sun_path);
    r = bind(listen_fd, (struct sockaddr *)&local, sizeof(local));
    if (r) {
        perror("failed to bind");
    }

    listen(listen_fd, 100);
    printf("starting request server...\n");

    for (;;) {
        socklen_t len = sizeof(remote);
        int remote_fd = accept(listen_fd, (struct sockaddr *)&remote, &len);
        if (remote_fd < 0) {
            perror("failed to accept");
            return 0;
        }
        uint8_t image_id[IMAGE_ID_SIZE];
        uint8_t * create_req_metadata;

        uint8_t buf[1024]; //TODO サイズを決める
        memset(buf, 0, sizeof buf);

        int n;
        n = read(listen_fd, buf, sizeof buf);
        if (n < 0) {
            perror("read length is zero");
            return 1;
        }

        sgx_enclave_id_t vm_enclave_id;

        //memcpy(vm_enclave_id, buf, sizeof sgx_enclave_id_t); // enclave_idを受け取る必要はない
        sgx_status_t status;
        status = create_enclave(&image_id, );
        if (status!=SGX_SUCCESS) {
            printf("create_enclave Ecall failed: Error code is %x", status);
            return -1;
        }

        int create_req_metadata_size = n - IMAGE_ID_SIZE;
        create_req_metadata = (uint8_t*)malloc(create_req_metadata_size);
        memcpy(create_req_metadata, &buf[IMAGE_ID_SIZE], create_req_metadata_size);

        status = MasterEnclave_LA(
            master_enclave_id, 
            &ret_status,
            master_enclave_id,
            &vm_enclave_id,
            create_req_metadata,
            create_req_metadata_size
        );

        status = MasterEnclave_test_create_session(master_enclave_id, &ret_status, master_enclave_id, vm_enclave_id);
        if (status!=SGX_SUCCESS) {
            printf("Enclave1_test_create_session Ecall failed: Error code is %x", status);
            break;
        } else {
            if(ret_status==0) {
                printf("\n\nSecure Channel Establishment between Source (E1) and Destination (E2) Enclaves successful !!!");
            } else {
                printf("\nSession establishment and key exchange failure between Source (E1) and Destination (E2): Error code is %x", ret_status);
                break;
            }
        }
        close(remote_fd);
    }
    close(listen_fd);
}

uint32_t load_enclaves()
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
    g_enclave_id_map.insert(std::pair<sgx_enclave_id_t, uint32_t>(master_enclave_id, enclave_temp_no));

    ret = sgx_create_enclave(GRAPHENE_ENC_PATH, SGX_DEBUG_FLAG, &launch_token, &launch_token_updated, &graphene_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
                return ret;
    }

    enclave_temp_no++;
    g_enclave_id_map.insert(std::pair<sgx_enclave_id_t, uint32_t>(graphene_enclave_id, enclave_temp_no));

    return SGX_SUCCESS;
}

int _tmain(int argc, _TCHAR* argv[])
{
    uint32_t ret_status;
    sgx_status_t status;

    UNUSED(argc);
    UNUSED(argv);

    if(load_enclaves() != SGX_SUCCESS)
    {
        printf("\nLoad Enclave Failure");
    }

    printf("\nAvailable Enclaves\n");
    printf("\master enclave - EnclaveID %" PRIx64, master_enclave_id);
    printf("\ngraphene enclave - EnclaveID %" PRIx64, graphene_enclave_id);
    printf("\n");

    run_request_server();
    
    // request_serverへ移行
    do
    {
        // 必要なのは master E と graphene E とのコネクションだけ
        // ファイル名＋関数名 どうやってる？
        //Test Create session between Enclave1(Source) and Enclave2(Destination)
        status = MasterEnclave_test_create_session(master_enclave_id, &ret_status, master_enclave_id, graphene_enclave_id);
        if (status!=SGX_SUCCESS)
        {
            printf("Enclave1_test_create_session Ecall failed: Error code is %x", status);
            break;
        }
        else
        {
            if(ret_status==0)
            {
                printf("\n\nSecure Channel Establishment between Source (E1) and Destination (E2) Enclaves successful !!!");
            }
            else
            {
                printf("\nSession establishment and key exchange failure between Source (E1) and Destination (E2): Error code is %x", ret_status);
                break;
            }
        }

        // //Test Enclave to Enclave call between Enclave1(Source) and Enclave2(Destination)
        // status = MasterEnclave_test_enclave_to_enclave_call(master_enclave_id, &ret_status, master_enclave_id, graphene_enclave_id);
        // if (status!=SGX_SUCCESS)
        // {
        //     printf("Enclave1_test_enclave_to_enclave_call Ecall failed: Error code is %x", status);
        //     break;
        // }
        // else
        // {
        //     if(ret_status==0)
        //     {
        //         printf("\n\nEnclave to Enclave Call between Source (E1) and Destination (E2) Enclaves successful !!!");
        //     }
        //     else
        //     {
        //         printf("\n\nEnclave to Enclave Call failure between Source (E1) and Destination (E2): Error code is %x", ret_status);
        //         break;
        //     }
        // }
        // //Test message exchange between Enclave1(Source) and Enclave2(Destination)
        // status = MasterEnclave_test_message_exchange(master_enclave_id, &ret_status, master_enclave_id, graphene_enclave_id);
        // if (status!=SGX_SUCCESS)
        // {
        //     printf("Enclave1_test_message_exchange Ecall failed: Error code is %x", status);
        //     break;
        // }
        // else
        // {
        //     if(ret_status==0)
        //     {
        //         printf("\n\nMessage Exchange between Source (E1) and Destination (E2) Enclaves successful !!!");
        //     }
        //     else
        //     {
        //         printf("\n\nMessage Exchange failure between Source (E1) and Destination (E2): Error code is %x", ret_status);
        //         break;
        //     }
        // }

        status = GrapheneEnclave_test_enclave_to_enclave_call(graphene_enclave_id, &ret_status, master_enclave_id, graphene_enclave_id);
        if (status!=SGX_SUCCESS)
        {
            printf("GrapheneEnclave_test_enclave_to_enclave_call failed: Error code is %x", status);
            break;
        }

        printf("ret: %x", ret_status);

        // //Test Closing Session between Enclave1(Source) and Enclave2(Destination)
        // status = MasterEnclave_test_close_session(master_enclave_id, &ret_status, master_enclave_id, graphene_enclave_id);
        // if (status!=SGX_SUCCESS)
        // {
        //     printf("Enclave1_test_close_session Ecall failed: Error code is %x", status);
        //     break;
        // }
        // else
        // {
        //     if(ret_status==0)
        //     {
        //         printf("\n\nClose Session between Source (E1) and Destination (E2) Enclaves successful !!!");
        //     }
        //     else
        //     {
        //         printf("\n\nClose session failure between Source (E1) and Destination (E2): Error code is %x", ret_status);
        //         break;
        //     }
        // }
    }while(0);

    waitForKeyPress();

    sgx_destroy_enclave(master_enclave_id);
    sgx_destroy_enclave(graphene_enclave_id);

    return 0;
}
