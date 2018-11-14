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
#include <stdio.h>      /* vsnprintf */
#include <string.h>      /* vsnprintf */

#include "MasterEnclave.h"
#include "MasterEnclave_t.h"  /* print_string */

#include "sgx_trts.h"
#include "sgx_utils.h"
#include "sgx_eid.h"
#include "sgx_ecp_types.h"
#include "sgx_thread.h"
#include <map>
#include "sgx_dh.h"
#include "sgx_tcrypto.h"

#include "datatypes.h"
#include "error_codes.h"

sgx_dh_session_t sgx_dh_session;

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
void printf(const char *fmt, ...)
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
        printf("%x ", str[i]);
    }
    printf("\n");
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

//LIB //Handle the request from Source Enclave for a session
ATTESTATION_STATUS ecall_session_request(sgx_dh_msg1_t * dh_msg1)
{
    sgx_status_t status = SGX_SUCCESS;

    if(!dh_msg1)
    {
        return INVALID_PARAMETER_ERROR;
    }
    //Intialize the session as a session responder
    status = sgx_dh_init_session(SGX_DH_SESSION_RESPONDER, &sgx_dh_session);
    if(SGX_SUCCESS != status)
    {
        return status;
    }

    //Generate Message1 that will be returned to Source Enclave
    status = sgx_dh_responder_gen_msg1((sgx_dh_msg1_t*)dh_msg1, &sgx_dh_session);
    if(SGX_SUCCESS != status)
    {
        return status;
    }

    return status;
}

//Verify Message 2, generate Message3 and exchange Message 3 with Source Enclave
ATTESTATION_STATUS ecall_exchange_report(
                          sgx_dh_msg2_t dh_msg2,
                          sgx_dh_msg3_t *dh_msg3,
                          la_arg_t la_arg)
{

    sgx_key_128bit_t dh_aek;   // Session key
    ATTESTATION_STATUS status = SUCCESS;
    sgx_dh_session_enclave_identity_t initiator_identity;

    if(!dh_msg3) // dh_msg2 || 
    {
        return INVALID_PARAMETER_ERROR;
    }

    memset(&dh_aek,0, sizeof(sgx_key_128bit_t));

    dh_msg3->msg3_body.additional_prop_length = 0;
    //Process message 2 from source enclave and obtain message 3
    sgx_status_t se_ret = sgx_dh_responder_proc_msg2(&dh_msg2, 
                                                    dh_msg3, 
                                                    &sgx_dh_session, 
                                                    &dh_aek, 
                                                    &initiator_identity);
    if(SGX_SUCCESS != se_ret)
    {
        return se_ret;
    }

    image_metadata_t imd;
    memset(&imd, 0, sizeof(image_metadata_t));
    memcpy(&imd, (const void *)la_arg.imd, sizeof(image_metadata_t));

    /* imd と crm の検証 */
    printf("--- reported enclave ---\n");
    print_hex((unsigned char *)&dh_msg2.report.body.mr_enclave, sizeof(sgx_measurement_t));

    printf("--- image enclave ---\n");
    print_hex((unsigned char *)&imd.mrenclave, sizeof(sgx_measurement_t));

    printf("--- imd ---\n");
    print_hex((unsigned char *)&imd, sizeof(image_metadata_t));

    //Verify source enclave's trust
    if(verify_peer_enclave_trust(&initiator_identity) != SUCCESS)
    {
        return INVALID_SESSION;
    }

    // if(status != SUCCESS)
    // {
    //     end_session(src_enclave_id);
    // }

    return status;
}
