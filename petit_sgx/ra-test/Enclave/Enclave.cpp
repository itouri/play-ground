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

#include "Enclave.h"
#include "Enclave_t.h"  /* print_string */

#include "tmessage.h"

#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/obj_mac.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>

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
    ocall_print_string(buf);
}


// void ecall_read_file_test(){

// }


static void
printHex(const char *title, const unsigned char *s, int len) 
{
	int    n;
	printf("%s:", title);
	for (n = 0; n < len; ++n) {
		if ((n % 16) == 0) {
			printf("\n%04x", n);
		}
		printf(" %02x", s[n]);
	}
	printf("\n");
}

int sha256_digest(const unsigned char *msg, size_t mlen, unsigned char digest[32])
{
	EVP_MD_CTX *ctx;

	memset(digest, 0, 32);

	//ctx = EVP_MD_CTX_create(); 
	ctx = EVP_MD_CTX_new(); //これでは動かない
	if ( ctx == NULL ) {
		goto cleanup;
	}

	if ( EVP_DigestInit(ctx, EVP_sha256()) != 1 ) {
		goto cleanup;
	}

	if ( EVP_DigestUpdate(ctx, msg, mlen) != 1 ) {
		goto cleanup;
	}

	if ( EVP_DigestFinal(ctx, digest, NULL) != 1 ) {
		goto cleanup;
	}

cleanup:
	if ( ctx != NULL ) EVP_MD_CTX_free(ctx);
	return -1;
}

void ecall_test(unsigned char *enc_data, int enc_len, dec_req_data_t *ret_req_data, unsigned char* prv_key_buf, long len) {
    char errbuf[1024];

	RSA * prv_key;
    prv_key = d2i_RSAPrivateKey(NULL, (const unsigned char**)&prv_key_buf, len);
	if( prv_key == NULL){
        printf("failed d2i_RSAPrivateKey :%s\n", ERR_error_string(ERR_get_error(), errbuf));
        return;
    }

    // int i;
	// for (i=0; i<len; i++) {
	// 	//printf("%x " ,(unsigned char)prv_key[i]);
	// }
	// printf("\n------------\n");

    printf("end i2d_RSAPrivateKey\n");

    int ret;
    /* decrypt */
    dec_req_data_t *deced_dec_req_data;
    int decrypted_len = 0;

	unsigned char decrypted_buf[1024];

    decrypted_len = RSA_private_decrypt(enc_len, enc_data, decrypted_buf, prv_key, RSA_PKCS1_PADDING);
    if ( decrypted_len == -1 ){
        printf("in decrypt: err=%s\n", ERR_error_string(ERR_get_error(), errbuf));
		return;
    }
    printf("end Decrypt\n");

	printf("plain_text: %s\n", decrypted_buf);

    // // 復号化した構造体のhashを計算して正しいか比較
    // unsigned char digest[32];
    // ret = sha256_digest((const unsigned char*)&deced_dec_req_data->req_data, sizeof(req_data_t), digest);
    // // TODO 1なら success?
    // if (!ret) {
    //     printf("failed sha256_digest");
    //     return;
    // }
    // printf("end sha256_digest\n");

    // // hashを比較
    // ret = memcmp(digest, deced_dec_req_data->digest, sizeof(digest));
    // if (!ret) {
    //     printf("hash is worng\n");
    //     return;
    // }

    // memcpy(ret_req_data, &deced_dec_req_data, sizeof(dec_req_data_t));

    printf("OK!\n");
}
