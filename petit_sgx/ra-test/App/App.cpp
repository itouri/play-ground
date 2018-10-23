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


#include <stdio.h>
#include <string.h>
#include <assert.h>

# include <unistd.h>
# include <pwd.h>
# define MAX_PATH FILENAME_MAX

#include "sgx_urts.h"
#include "App.h"
#include "Enclave_u.h"

#include "crypto.h"
#include "message.h"
#include "ucrypto.h"

#include <openssl/err.h>
#include <openssl/rand.h>
#include <uuid/uuid.h>

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

EVP_PKEY *test_prv_pkey;
EVP_PKEY *test_pub_pkey;

// 読んだ鍵
EC_KEY *test_prv_key;
//EC_KEY *test_pub_key;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid SGX device.",
        "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
};

// TODO
int elgamal_encrypt(unsigned char **encData, unsigned char *data, int dataLen, const EC_KEY *eckey) 
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


/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
    	printf("Error code is 0x%X. Please refer to the \"Intel SGX SDK Developer Reference\" for more details.\n", ret);
}

/* Initialize the enclave:
 *   Step 1: try to retrieve the launch token saved by last transaction
 *   Step 2: call sgx_create_enclave to initialize an enclave instance
 *   Step 3: save the launch token if it is updated
 */
int initialize_enclave(void)
{
    char token_path[MAX_PATH] = {'\0'};
    sgx_launch_token_t token = {0};
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    int updated = 0;
    
    /* Step 1: try to retrieve the launch token saved by last transaction 
     *         if there is no token, then create a new one.
     */
    /* try to get the token saved in $HOME */
    const char *home_dir = getpwuid(getuid())->pw_dir;
    
    if (home_dir != NULL && 
        (strlen(home_dir)+strlen("/")+sizeof(TOKEN_FILENAME)+1) <= MAX_PATH) {
        /* compose the token path */
        strncpy(token_path, home_dir, strlen(home_dir));
        strncat(token_path, "/", strlen("/"));
        strncat(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME)+1);
    } else {
        /* if token path is too long or $HOME is NULL */
        strncpy(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME));
    }

    FILE *fp = fopen(token_path, "rb");
    if (fp == NULL && (fp = fopen(token_path, "wb")) == NULL) {
        printf("Warning: Failed to create/open the launch token file \"%s\".\n", token_path);
    }

    if (fp != NULL) {
        /* read the token from saved file */
        size_t read_num = fread(token, 1, sizeof(sgx_launch_token_t), fp);
        if (read_num != 0 && read_num != sizeof(sgx_launch_token_t)) {
            /* if token is invalid, clear the buffer */
            memset(&token, 0x0, sizeof(sgx_launch_token_t));
            printf("Warning: Invalid launch token read from \"%s\".\n", token_path);
        }
    }
    /* Step 2: call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL);
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        if (fp != NULL) fclose(fp);
        return -1;
    }

    /* Step 3: save the launch token if it is updated */
    if (updated == FALSE || fp == NULL) {
        /* if the token is not updated, or file handler is invalid, do not perform saving */
        if (fp != NULL) fclose(fp);
        return 0;
    }

    /* reopen the file with write capablity */
    fp = freopen(token_path, "wb", fp);
    if (fp == NULL) return 0;
    size_t write_num = fwrite(token, 1, sizeof(sgx_launch_token_t), fp);
    if (write_num != sizeof(sgx_launch_token_t))
        printf("Warning: Failed to save launch token to \"%s\".\n", token_path);
    fclose(fp);
    return 0;
}



/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
}

int read_ec_keys(){

    const char *prv_file_path = "ec-key.pem";
    if (!key_load_file(&test_prv_pkey, prv_file_path, KEY_PRIVATE)) {
        printf("can't load key private key file: %s\n", prv_file_path);
        return -1;
    }

    const char *pub_file_path = "ec-key-pub.pem";
    if (!key_load_file(&test_pub_pkey, pub_file_path, KEY_PUBLIC)) {
        printf("can't load key public key file: %s\n", pub_file_path);
        return -1;
    }

    if ( (test_prv_key = EC_KEY_new()) == NULL ) {
        printf("failed test_prv_key = EC_KEY_new()\n");
        return -1;
    }

    // 読み込んだ鍵から EC_KEY を取り出す
    test_prv_key = EVP_PKEY_get1_EC_KEY(test_prv_pkey);
    if ( test_prv_key == NULL ) {
        printf("cant read test_prv_key\n");
        return -1;

    }

    return 0;
}

/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);

    /* Initialize the enclave */
    if(initialize_enclave() < 0){
        printf("Enter a character before exit ...\n");
        getchar();
        return -1; 
    }

    /* read key */
    if ( !read_ec_keys() ) {
        return -1;
    }

    /* init req_data */
    req_data_t *req_data;
	req_data = (req_data_t*)malloc(sizeof(req_data_t));
    uuid_generate(req_data->client_id);
    
    int ret;
    ret = RAND_bytes(req_data->nonce, sizeof req_data->nonce);
    unsigned long err = ERR_get_error();
    if(ret != 1) {
        /* RAND_bytes failed */
        /* `err` is valid    */
        printf("RAND_bytes is failed\n");
        return 1;
    }

    /* int dec_req_data_t */
    dec_req_data_t *dec_req_data;
    dec_req_data = (dec_req_data_t*)malloc(sizeof(dec_req_data_t));
    memcpy(&dec_req_data->req_data, req_data, sizeof(req_data_t));

    // hashを計算して追加
    ret = sha256_digest((const unsigned char*)&dec_req_data->req_data, sizeof(req_data_t), dec_req_data->digest);
    // TODO 1なら success?
    if (!ret) {
        printf("failed sha256_digest");
        return 1;
    }

    /* encrypt */
    unsigned char *enc_data;
    int enc_len = 0;
    enc_len = elgamal_encrypt((unsigned char**)&enc_data, (unsigned char*)req_data, sizeof req_data, test_prv_key);
    if (enc_len==0) {
        printf("Encrypt error\n");
		return 1;
    }

    dec_req_data_t *ret_dec_req_data;
    ret_dec_req_data = (dec_req_data_t*)malloc(sizeof(dec_req_data_t));
    // ecallで enc_dataを渡してしまう
    //int sz = BN_num_bits( test_pub_key->group->p );
    int sz = EVP_PKEY_size(test_pub_pkey);
    ecall_test(global_eid, enc_data, enc_len, ret_dec_req_data, (unsigned char*)test_pub_pkey, sz);

    // 復号化した構造体のhashを計算
    unsigned char digest[32];
    ret = sha256_digest((const unsigned char*)&ret_dec_req_data->req_data, sizeof(req_data_t), digest);
    // TODO 1なら success?
    if (!ret) {
        printf("failed sha256_digest");
        return 1;
    }

    // hashを比較
    ret = memcmp(digest, ret_dec_req_data->digest, sizeof(digest));
    if (!ret) {
        printf("hash is worng\n");
        return 1;
    }

    // 最初の構造体と同じになっているか比較
    ret = memcmp(dec_req_data, ret_dec_req_data, sizeof(req_data_t));
    if (!ret) {
        printf("failed memcmp\n");
        return 1;
    }    

    /* Destroy the enclave */
    sgx_destroy_enclave(global_eid);
    
    printf("Info: SampleEnclave successfully returned.\n");

    printf("Enter a character before exit ...\n");
    getchar();
    return 0;
}

// // localfileのread
// int read_file_ocall(unsigned char *dest, char *file, off_t *len) {
// 	if ( !from_file(dest, file ,len) ) {
// 		// error
// 		fprintf(stderr, "failed read_file_ocall()\n");
// 		return 1;
// 	}
// }