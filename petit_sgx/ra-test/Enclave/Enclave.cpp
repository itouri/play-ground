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


void ecall_read_file_test(){

}


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

/**
 * Encryption
 */
int
elgamal_encrypt(unsigned char **encData, unsigned char *data, int dataLen, const EC_KEY *eckey) 
{
	BN_CTX *ctx = NULL;
	BIGNUM *r = NULL, *p = NULL, *m;
	EC_POINT *C1 = NULL, *C2 = NULL;
	EC_POINT *Tmp = NULL, *M;
	const EC_POINT *Pkey;
	const EC_GROUP *group;
	int    c1Len, c2Len;
	int    rv;

	if ((group = EC_KEY_get0_group(eckey)) == NULL) {
		return 0;
	}
	p = BN_new();
	ctx = BN_CTX_new();
	EC_GROUP_get_curve_GFp(group, p, NULL, NULL, ctx);
#ifdef DEBUG
	printf(" p = ");
	BN_print_fp(stdout, p);
	puts("");
#endif

	/* C1 = r*G */
	C1 = EC_POINT_new(group);

	/* generate random number r */ 
	r = BN_new();
	M = EC_POINT_new(group);
	m = BN_new();
	do {
		if (!BN_rand_range(r, p)) {
			return 0;
		}
	} while (BN_is_zero(r));
#ifdef DEBUG
	printf(" r = ");
	BN_print_fp(stdout, r);
	puts("");
#endif

	EC_POINT_mul(group, C1, r, NULL, NULL, ctx);

	/* C2 = r*P + M */ 
	/* M */
	BN_bin2bn(data, dataLen, m);
	rv = EC_POINT_set_compressed_coordinates_GFp(group, M, m, 1, ctx);
	if (!rv) {
		return 0;
	}

	C2 = EC_POINT_new(group);
	Tmp = EC_POINT_new(group);
	Pkey = EC_KEY_get0_public_key(eckey);
	EC_POINT_mul(group, Tmp, NULL, Pkey, r, ctx);
	EC_POINT_add(group, C2, Tmp, M, ctx);

	/* cipher text C = (C1, C2) */ 
	c1Len = EC_POINT_point2oct(group, C1, POINT_CONVERSION_COMPRESSED,
							   NULL, 0, ctx);
#ifdef DEBUG
	printf(" Point converted length (C1) = %d\n", c1Len);
#endif
	c2Len =	EC_POINT_point2oct(group, C2, POINT_CONVERSION_COMPRESSED,
							   NULL, 0, ctx);
#ifdef DEBUG
	printf(" Point converted length (C2) = %d\n", c1Len);
#endif
	*encData = (unsigned char*)OPENSSL_malloc(c1Len + c2Len);
	EC_POINT_point2oct(group, C1, POINT_CONVERSION_COMPRESSED,
							*encData, c1Len, ctx);
	EC_POINT_point2oct(group, C2, POINT_CONVERSION_COMPRESSED,
							*encData + c1Len, c2Len, ctx);

	BN_clear_free(p);
	BN_clear_free(r);
	BN_clear_free(m);
	EC_POINT_free(C1);
	EC_POINT_free(C2);
	EC_POINT_free(M);
	EC_POINT_free(Tmp);
	BN_CTX_free(ctx);

	return (c1Len + c2Len);
}

/**
 * Decryption
 */
int elgamal_decrypt(unsigned char **decData, unsigned char *encData, int encLen, const EC_KEY *eckey) 
{
	int rv;
	const EC_GROUP *group;
	const BIGNUM *prvKey;
	BN_CTX *ctx;
	EC_POINT *C1 = NULL, *C2 = NULL;
	EC_POINT *M = NULL, *Tmp = NULL;

	group = EC_KEY_get0_group(eckey);
	prvKey = EC_KEY_get0_private_key(eckey);
#ifdef DEBUG
	printf(" prvKey = ");
	BN_print_fp(stdout, prvKey);
	puts("");
#endif
	C1 = EC_POINT_new(group);
	C2 = EC_POINT_new(group);
	ctx = BN_CTX_new();

	/* C1 */
#ifdef DEBUG
	printHex("C1", encData, encLen / 2);
#endif
	rv = EC_POINT_oct2point(group, C1, encData, encLen / 2, ctx);
	if (!rv) {
		printf("EC_POINT_oct2point error (C1)\n");
		return 0;
	}

	/* C2 */
#ifdef DEBUG
	printHex("C2", encData + encLen / 2, encLen / 2);
#endif
	rv = EC_POINT_oct2point(group, C2, encData + encLen / 2, encLen / 2,
							ctx);
	if (!rv) {
		printf("EC_POINT_oct2point error (C2)\n");
		return 0;
	}
	Tmp = EC_POINT_new(group);
	M = EC_POINT_new(group);

	/* M = C2 - x C1 */ 
	EC_POINT_mul(group, Tmp, NULL, C1, prvKey, ctx);
	EC_POINT_invert(group, Tmp, ctx);
	EC_POINT_add(group, M, C2, Tmp, ctx);

	/* Output M */ 
	rv = EC_POINT_point2oct(group, M, POINT_CONVERSION_COMPRESSED, NULL, 0,
							ctx);

#ifdef DEBUG
	printf(" Point converted length = %d\n", rv);
#endif
	*decData = (unsigned char*)OPENSSL_malloc(rv);
	EC_POINT_point2oct(group, M, POINT_CONVERSION_COMPRESSED, *decData,
					   rv, ctx);

	EC_POINT_free(C1);
	EC_POINT_free(C2);
	EC_POINT_free(M);
	EC_POINT_free(Tmp);
	BN_CTX_free(ctx);

	return rv;
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

void ecall_test(unsigned char *enc_data, int enc_len, dec_req_data_t *ret_req_data, unsigned char* test_prv_key, int len) {
    // EVP_PKEY *opkey;
	// opkey = EVP_PKEY_new();
	// if(opkey == NULL){
    //     printf("EVP_PKEY_new()\n"); 
    //     return;
    // }
    EC_KEY *eckey;
    if ( (eckey = ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime192v1)) == NULL ) {
        printf("failed test_prv_key = EC_KEY_new()\n");
        return;
    }

    EC_KEY_set_asn1_flag(ec_key, OPENSSL_EC_NAMED_CURVE);

	if(d2i_ECPrivateKey(&eckey, (const unsigned char**)&test_prv_key, len) != NULL){
        printf("d2i_PUBKEY(&opkey, &pub_key, len)\n"); 
        return;
    }

    int i;
    for (i=0; i < len; i++) {
        printf("%x ", test_prv_key[i]);
    }
    printf("\n");

    printf("end d2i_ECPrivateKey\n");

    int ret;
    /* decrypt */
    dec_req_data_t *deced_dec_req_data;
    int dec_len = 0;

    dec_len = elgamal_decrypt((unsigned char**)&deced_dec_req_data, enc_data, enc_len, eckey);
    if (!dec_len) {
        printf("Decrypt error\n");
		return;
    }
    printf("end Decrypt\n");

    // 復号化した構造体のhashを計算して正しいか比較
    unsigned char digest[32];
    ret = sha256_digest((const unsigned char*)&deced_dec_req_data->req_data, sizeof(req_data_t), digest);
    // TODO 1なら success?
    if (!ret) {
        printf("failed sha256_digest");
        return;
    }
    printf("end sha256_digest\n");

    // hashを比較
    ret = memcmp(digest, deced_dec_req_data->digest, sizeof(digest));
    if (!ret) {
        printf("hash is worng\n");
        return;
    }

    memcpy(ret_req_data, &deced_dec_req_data, sizeof(dec_req_data_t));

    printf("OK!\n");
}
