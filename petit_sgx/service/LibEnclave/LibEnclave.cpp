/*
 * Copyright (C) 2011-2017 Intel Corporation. All rights reserved.
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


#include <stdarg.h>
#include <stdio.h>      /* vsnlib_printf */
#include <string.h>      /* vsnlib_printf */

#include "sgx_trts.h"
#include "sgx_utils.h"
#include "sgx_eid.h"
#include "sgx_ecp_types.h"
#include "sgx_thread.h"
#include <map>
#include "sgx_dh.h"
#include "sgx_tcrypto.h"

#include "LibEnclave.h"
#include "LibEnclave_t.h"  /* print_string */

#include "error_codes.h"

/* 
 * lib_printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
void lib_printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print(buf);
}

void print_hex(unsigned char * str, size_t size) {
    int i;
    for (i=0; i<size; i++) {
        lib_printf("%02x", str[i]);
    }
}

//Function that is used to verify the trust of the other enclave
//Each enclave can have its own way verifying the peer enclave identity
extern "C" uint32_t verify_peer_enclave_trust(sgx_dh_session_enclave_identity_t* peer_enclave_identity)
{
    if(!peer_enclave_identity)
    {
        return INVALID_PARAMETER_ERROR;
    }
    if(peer_enclave_identity->isv_prod_id != 0 || !(peer_enclave_identity->attributes.flags & SGX_FLAGS_INITTED))
        // || peer_enclave_identity->attributes.xfrm !=3)// || peer_enclave_identity->mr_signer != xx //TODO: To be hardcoded with values to check
    {
        return ENCLAVE_TRUST_ERROR;
    }
    else
    {
        return SUCCESS;
    }
}

//Create a session with the destination enclave
//ATTESTATION_STATUS ecall_create_session()
ATTESTATION_STATUS create_session()
{
    sgx_dh_msg1_t dh_msg1;            //Diffie-Hellman Message 1
    sgx_key_128bit_t dh_aek;        // Session Key
    sgx_dh_msg2_t dh_msg2;            //Diffie-Hellman Message 2
    sgx_dh_msg3_t dh_msg3;            //Diffie-Hellman Message 3
    uint32_t retstatus;
    sgx_status_t status = SGX_SUCCESS;
    sgx_dh_session_t sgx_dh_session;
    sgx_dh_session_enclave_identity_t responder_identity;

    memset(&dh_aek,0, sizeof(sgx_key_128bit_t));
    memset(&dh_msg1, 0, sizeof(sgx_dh_msg1_t));
    memset(&dh_msg2, 0, sizeof(sgx_dh_msg2_t));
    memset(&dh_msg3, 0, sizeof(sgx_dh_msg3_t));

    //Intialize the session as a session initiator
    status = sgx_dh_init_session(SGX_DH_SESSION_INITIATOR, &sgx_dh_session);
    if(SGX_SUCCESS != status)
    {
        lib_printf("failed sgx_dh_init_session\n");
        return status;
    }

    //Ocall to request for a session with the destination enclave and obtain session id and Message 1 if successful
    status = ocall_session_request(&retstatus, &dh_msg1);
    if (status == SGX_SUCCESS) {
        if ((ATTESTATION_STATUS)retstatus != SUCCESS)
            return ((ATTESTATION_STATUS)retstatus);
    } else {
        return ATTESTATION_SE_ERROR;
    }

    //print_hex((uint8_t*)&dh_msg1, sizeof(sgx_dh_msg1_t));
    //Process the message 1 obtained from desination enclave and generate message 2
    status = sgx_dh_initiator_proc_msg1(&dh_msg1, &dh_msg2, &sgx_dh_session);
    if(SGX_SUCCESS != status)
    {
        lib_printf("failed sgx_dh_initiator_proc_msg1?: %x\n", status);
        return status;
    }

    //Send Message 2 to Destination Enclave and get Message 3 in return
    status = ocall_exchange_report(&retstatus, dh_msg2, &dh_msg3);
    if (status == SGX_SUCCESS)
    {
        if ((ATTESTATION_STATUS)retstatus != SUCCESS)
            return ((ATTESTATION_STATUS)retstatus);
    }
    else
    {
        return ATTESTATION_SE_ERROR;
    }

    //Process Message 3 obtained from the destination enclave
    status = sgx_dh_initiator_proc_msg3(&dh_msg3, &sgx_dh_session, &dh_aek, &responder_identity);
    if(SGX_SUCCESS != status)
    {
        lib_printf("failed sgx_dh_initiator_proc_msg3\n");
        return status;
    }

    // MasterEncの処理 送られて来たMRENCLAVEが正しいgrapheneのものか
    //print_ocall((char*)&dh_msg3.msg3_body.report.body.mr_enclave);

    // Verify the identity of the destination enclave
    if(verify_peer_enclave_trust(&responder_identity) != SUCCESS)
    {
        return INVALID_SESSION;
    }
    return status;
}

void get_mr_enclave () {
    sgx_report_t report;
    sgx_status_t ret;

    //printe("begin sgx_create_report\n");
    ret = sgx_create_report(NULL, NULL, &report);
    if ( ret != SGX_SUCCESS )
    {
        lib_printf("failed sgx_create_repor: %x\n", ret);
        // return ret;
    }
    print_hex((unsigned char*)&report.body.mr_enclave, sizeof(sgx_measurement_t));
}

void why () {
    lib_printf("???\n");
}