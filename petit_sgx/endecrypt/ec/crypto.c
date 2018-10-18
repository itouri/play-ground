/*

Copyright 2018 Intel Corporation

This software and the related documents are Intel copyrighted materials,
and your use of them is governed by the express license under which they
were provided to you (License). Unless the License provides otherwise,
you may not use, modify, copy, publish, distribute, disclose or transmit
this software or the related documents without Intel's prior written
permission.

This software and the related documents are provided as is, with no
express or implied warranties, other than those that are expressly stated
in the License.

*/

#include <openssl/cmac.h>
#include <openssl/conf.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bn.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <string.h>
#include <stdio.h>
//#include <sgx_key_exchange.h>
#include "crypto.h"
#include "hexutil.h"

static enum _error_type {
	e_none,
	e_crypto,
	e_system,
	e_api
} error_type= e_none;

static const char *ep= NULL;

void crypto_init ()
{
	/* Load error strings for libcrypto */
	ERR_load_crypto_strings();

	/* Load digest and ciphers */
	OpenSSL_add_all_algorithms();
}

void crypto_destroy ()
{
	EVP_cleanup();

	CRYPTO_cleanup_all_ex_data();

	ERR_free_strings();
}

/* Print the error */

void crypto_perror (const char *prefix)
{
	fprintf(stderr, "%s: ", prefix);
	if ( error_type == e_none ) fprintf(stderr, "no error\n");
	else if ( error_type == e_system ) perror(ep);
	else if ( error_type == e_crypto ) ERR_print_errors_fp(stderr);
	else if ( error_type == e_api ) fprintf(stderr, "invalid parameter\n");
	else fprintf(stderr, "unknown error\n");
}

/*==========================================================================
 * EC key functions 
 *========================================================================== */

/* Load an EC key from a file in PEM format */

// int key_load (EVP_PKEY **pkey, const char *hexstring, int keytype)
// {
// 	EC_KEY *eckey= NULL;
// 	BIGNUM *gx= NULL;
// 	BIGNUM *gy= NULL;
// 	size_t slen, reqlen;

// 	error_type= e_none;

// 	/* Make sure we were sent a proper hex string for a key */
// 	if ( hexstring == NULL ) {
// 		error_type= e_api;
// 		return 0;
// 	}

// 	slen= strlen(hexstring);
// 	if ( keytype == KEY_PRIVATE ) reqlen=64;
// 	else if ( keytype == KEY_PUBLIC ) reqlen= 128;
// 	else {
// 		error_type= e_api;
// 		return 0;
// 	}
// 	if ( slen != reqlen ) {
// 		error_type= e_api;
// 		return 0;
// 	}

// 	// 楕円曲線 ID nid を用いて EC_KEY を生成する．
// 	eckey= EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
// 	if ( eckey == NULL ) {
// 		error_type= e_crypto;
// 		goto cleanup;
// 	}

// 	if ( keytype == KEY_PRIVATE ) {
// 		EC_POINT *pubpt= NULL;
// 		const EC_GROUP *group= NULL;

// 		// BN: BIGNUM 暗号で使われる大きな数を扱うライブラリ
// 		BN_CTX *ctx;

// 		ctx= BN_CTX_new();

// 		/* hexstring is the private key; we'll use gx even though that's
// 		 * not technically what it is. :)  */

// 		if ( ! BN_hex2bn(&gx, hexstring) ) {
// 			error_type= e_crypto;
// 			goto cleanup;
// 		}

// 		if ( ! EC_KEY_set_private_key(eckey, gx) ) {
// 			error_type= e_crypto;
// 			goto cleanup;
// 		}

// 		/* Set the public key from the private key */

// 		group= EC_KEY_get0_group(eckey);

// 		pubpt= EC_POINT_new(group);
// 		if ( pubpt == NULL ) {
// 			BN_CTX_free(ctx);
// 			error_type= e_crypto;
// 			goto cleanup;
// 		}

// 		if ( ! EC_POINT_mul(group, pubpt, gx, NULL, NULL,
// 			NULL) ) {

// 			BN_CTX_free(ctx);
// 			EC_POINT_free(pubpt);

// 			error_type= e_crypto;
// 			goto cleanup;
// 		}

// 		BN_CTX_free(ctx);

// 		if ( ! EC_KEY_set_public_key(eckey, pubpt) ) {
// 			EC_POINT_free(pubpt);

// 			EC_POINT_free(pubpt);

// 			error_type= e_crypto;
// 			goto cleanup;
// 		}

// 		EC_POINT_free(pubpt);
// 	} else if ( keytype == KEY_PUBLIC ) {
// 		/*
// 		 * hex2bn expects a NULL terminated string, so need to 
// 		 * pull out the x component
// 		 */

// 		char cx[65];

// 		memcpy(cx, hexstring, 64);
// 		cx[64]= 0;

// 		if ( ! BN_hex2bn(&gx, cx) ) {
// 			error_type= e_crypto;
// 			goto cleanup;
// 		}

// 		if ( ! BN_hex2bn(&gy, &hexstring[64]) ) {
// 			error_type= e_crypto;
// 			goto cleanup;
// 		}

// 		// これが mac でエラーになる
// 		if ( ! EC_KEY_set_public_key_affine_coordinates(eckey, gx, gy) ) {
// 			error_type= e_crypto;
// 			goto cleanup;
// 		}
		
// 	} else {
// 		error_type= e_api;
// 		goto cleanup;
// 	}

// 	*pkey= EVP_PKEY_new();
// 	if ( *pkey == NULL ) {
// 		error_type= e_crypto;
// 		goto cleanup;
// 	}

// 	if ( ! EVP_PKEY_set1_EC_KEY(*pkey, eckey) ) {
// 		error_type= e_crypto;
// 		*pkey= NULL;
// 	}

// cleanup:
// 	if ( gy != NULL ) BN_free(gy);
// 	if ( gx != NULL ) BN_free(gx);
// 	/* if ( eckey != NULL ) EC_KEY_free(eckey); */

// 	return (error_type == e_none);
// }

int key_load_file (EVP_PKEY **key, const char *filename, int keytype)
{
	FILE *fp;

	error_type= e_none;

	*key= EVP_PKEY_new();

	if ( (fp= fopen(filename, "r")) == NULL ) {
		error_type= e_system;
		ep= filename;
		return 0;
	}

	if ( keytype == KEY_PRIVATE ) PEM_read_PrivateKey(fp, key, NULL, NULL);
	else if ( keytype == KEY_PUBLIC ) PEM_read_PUBKEY(fp, key, NULL, NULL);
	else {
		error_type= e_api;
	}

	fclose(fp);

	return (error_type == e_none);
}
