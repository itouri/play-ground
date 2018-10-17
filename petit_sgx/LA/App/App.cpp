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
#include <map>
#include "../MasterEnclave/MasterEnclave_u.h"
#include "../GrapheneEnclave/GrapheneEnclave_u.h"
#include "sgx_eid.h"
#include "sgx_urts.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>


#define UNUSED(val) (void)(val)
#define TCHAR   char
#define _TCHAR  char
#define _T(str) str
#define scanf_s scanf
#define _tmain  main

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
