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
#include "sgx_tae_service.h"
#include "sgx_tkey_exchange.h"

#include "datatypes.h"
#include "error_codes.h"

sgx_dh_session_t sgx_dh_session;
unsigned long prvkey = -1;

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
        printf("%02d ", str[i]);
    }
    printf("\n");
}

void decrypt(unsigned char * msg, unsigned char * enc_msg, size_t msg_size, int n)
{
  int i, j;
  unsigned long k;

  if (prvkey == -1) {
      printf("prvkey is not setted!\n");
      return;
  }

  for ( i=0; i < msg_size; i++ ) {
      k = 1;
      for (j=0; j < prvkey; j++) {
          k *= enc_msg[i];
          k %= n;
      }
      msg[i] = k;
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
    // memcpy(&imd, (const void *)la_arg.imd, sizeof(image_metadata_t));
    decrypt((unsigned char*)&imd, (unsigned char*)la_arg.imd, la_arg.imd_sz, 253);

    create_req_data_t crm;
    memset(&crm, 0, sizeof(create_req_data_t));
    decrypt((unsigned char*)&crm, (unsigned char*)la_arg.crm, la_arg.crm_sz, 253);

    printf("\nfrom enc: %ld\n", prvkey);

    /* imd と crm の検証 */
    printf("--- reported enclave ---\n");
    print_hex((unsigned char *)&dh_msg2.report.body.mr_enclave, sizeof(sgx_measurement_t));

    printf("--- image enclave ---\n");
    print_hex((unsigned char *)&imd.mrenclave, sizeof(sgx_measurement_t));

    printf("--- imd ---\n");
    print_hex((unsigned char *)&imd, sizeof(image_metadata_t));

    printf("--- crm ---\n");
    print_hex((unsigned char *)&crm, sizeof(create_req_data_t));

    //Verify source enclave's trust
    if(verify_peer_enclave_trust(&initiator_identity) != SUCCESS)
    {
        return INVALID_SESSION;
    }

    if ( memcmp(&imd.client_id, &crm.client_id, sizeof(imd.client_id)) ) {
        printf("client id is not match\n");
    }

    if ( memcmp(&imd.mrenclave, &dh_msg2.report.body.mr_enclave, sizeof(imd.mrenclave)) ) {
        printf("mrenclave is not match\n");
    }

    // if(status != SUCCESS)
    // {
    //     end_session(src_enclave_id);
    // }

    return status;
}

// ############### ra ###############

static const sgx_ec256_public_t def_service_public_key = {
    {
        0x72, 0x12, 0x8a, 0x7a, 0x17, 0x52, 0x6e, 0xbf,
        0x85, 0xd0, 0x3a, 0x62, 0x37, 0x30, 0xae, 0xad,
        0x3e, 0x3d, 0xaa, 0xee, 0x9c, 0x60, 0x73, 0x1d,
        0xb0, 0x5b, 0xe8, 0x62, 0x1c, 0x4b, 0xeb, 0x38
    },
    {
        0xd4, 0x81, 0x40, 0xd9, 0x50, 0xe2, 0x57, 0x7b,
        0x26, 0xee, 0xb7, 0x41, 0xe7, 0xc6, 0x14, 0xe2,
        0x24, 0xb7, 0xbd, 0xc9, 0x03, 0xf2, 0x9a, 0x28,
        0xa8, 0x3c, 0xc8, 0x10, 0x11, 0x14, 0x5e, 0x06
    }

};

#define PSE_RETRIES	5	/* Arbitrary. Not too long, not too short. */

/*----------------------------------------------------------------------
 * WARNING
 *----------------------------------------------------------------------
 *
 * End developers should not normally be calling these functions
 * directly when doing remote attestation:
 *
 *    sgx_get_ps_sec_prop()
 *    sgx_get_quote()
 *    sgx_get_quote_size()
 *    sgx_get_report()
 *    sgx_init_quote()
 *
 * These functions short-circuits the RA process in order
 * to generate an enclave quote directly!
 *
 * The high-level functions provided for remote attestation take
 * care of the low-level details of quote generation for you:
 *
 *   sgx_ra_init()
 *   sgx_ra_get_msg1
 *   sgx_ra_proc_msg2
 *
 *----------------------------------------------------------------------
 */

/*
 * This doesn't really need to be a C++ source file, but a bug in 
 * 2.1.3 and earlier implementations of the SGX SDK left a stray
 * C++ symbol in libsgx_tkey_exchange.so so it won't link without
 * a C++ compiler. Just making the source C++ was the easiest way
 * to deal with that.
 */

sgx_status_t get_report(sgx_report_t *report, sgx_target_info_t *target_info)
{
#ifdef SGX_HW_SIM
	return sgx_create_report(NULL, NULL, report);
#else
	return sgx_create_report(target_info, NULL, report);
#endif
}

size_t get_pse_manifest_size ()
{
	return sizeof(sgx_ps_sec_prop_desc_t);
}

sgx_status_t get_pse_manifest(char *buf, size_t sz)
{
	sgx_ps_sec_prop_desc_t ps_sec_prop_desc;
	sgx_status_t status= SGX_ERROR_SERVICE_UNAVAILABLE;
	int retries= PSE_RETRIES;

	do {
		status= sgx_create_pse_session();
		if ( status != SGX_SUCCESS ) return status;
	} while (status == SGX_ERROR_BUSY && retries--);
	if ( status != SGX_SUCCESS ) return status;

	status= sgx_get_ps_sec_prop(&ps_sec_prop_desc);
	if ( status != SGX_SUCCESS ) return status;

	memcpy(buf, &ps_sec_prop_desc, sizeof(ps_sec_prop_desc));

	sgx_close_pse_session();

	return status;
}

sgx_status_t enclave_ra_init(sgx_ec256_public_t key, int b_pse,
	sgx_ra_context_t *ctx, sgx_status_t *pse_status)
{
	sgx_status_t ra_status;

	/*
	 * If we want platform services, we must create a PSE session 
	 * before calling sgx_ra_init()
	 */

	if ( b_pse ) {
		int retries= PSE_RETRIES;
		do {
			*pse_status= sgx_create_pse_session();
			if ( *pse_status != SGX_SUCCESS ) return SGX_ERROR_UNEXPECTED;
		} while (*pse_status == SGX_ERROR_BUSY && retries--);
		if ( *pse_status != SGX_SUCCESS ) return SGX_ERROR_UNEXPECTED;
	}

	ra_status= sgx_ra_init(&key, b_pse, ctx);

	if ( b_pse ) {
		int retries= PSE_RETRIES;
		do {
			*pse_status= sgx_create_pse_session();
			if ( *pse_status != SGX_SUCCESS ) return SGX_ERROR_UNEXPECTED;
		} while (*pse_status == SGX_ERROR_BUSY && retries--);
		if ( *pse_status != SGX_SUCCESS ) return SGX_ERROR_UNEXPECTED;
	}

	return ra_status;
}

sgx_status_t enclave_ra_init_def(int b_pse, sgx_ra_context_t *ctx,
	sgx_status_t *pse_status)
{
	return enclave_ra_init(def_service_public_key, b_pse, ctx, pse_status);
}

/*
 * Return a SHA256 hash of the requested key. KEYS SHOULD NEVER BE
 * SENT OUTSIDE THE ENCLAVE IN PLAIN TEXT. This function let's us
 * get proof of possession of the key without exposing it to untrusted
 * memory.
 */

sgx_status_t enclave_ra_get_key_hash(sgx_status_t *get_keys_ret,
	sgx_ra_context_t ctx, sgx_ra_key_type_t type, sgx_sha256_hash_t *hash)
{
	sgx_status_t sha_ret;
	sgx_ra_key_128_t k;

	// First get the requested key which is one of:
	//  * SGX_RA_KEY_MK 
	//  * SGX_RA_KEY_SK
	// per sgx_ra_get_keys().

	*get_keys_ret= sgx_ra_get_keys(ctx, type, &k);
	if ( *get_keys_ret != SGX_SUCCESS ) return *get_keys_ret;

	/* Now generate a SHA hash */

	sha_ret= sgx_sha256_msg((const uint8_t *) &k, sizeof(k), 
		(sgx_sha256_hash_t *) hash); // Sigh.

	/* Let's be thorough */

	memset(k, 0, sizeof(k));

	return sha_ret;
}

sgx_status_t enclave_ra_close(sgx_ra_context_t ctx)
{
        sgx_status_t ret;
        ret = sgx_ra_close(ctx);
        return ret;
}

void ecall_set_private_key(unsigned long key)
{
    prvkey = key;
}
