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
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <sys/stat.h>
#include <uuid/uuid.h>

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

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

RSA *pub_key;
RSA *prv_key;

int read_rsa_keys()
{
    /* key load */
    const char *pub_file_path = "rsa-key-pub.pem";
    const char *prv_file_path = "rsa-key.pem";

    FILE *pubkey_file;
    FILE *prvkey_file;

    char errbuf[1024];

    pubkey_file = fopen(pub_file_path, "r");
    if (pubkey_file == NULL) {
        printf("can't load pub key file: %s\n", ERR_error_string(ERR_get_error(), errbuf));
        return -1;
    }

    prvkey_file = fopen(prv_file_path, "r");
    if (prvkey_file == NULL) {
        printf("can't load prv key file: %s\n", ERR_error_string(ERR_get_error(), errbuf));
        return -1;
    }

    pub_key = PEM_read_RSAPublicKey(pubkey_file, NULL, NULL, NULL);
    if (pub_key == NULL) {
        printf("can't load pub key: %s\n", ERR_error_string(ERR_get_error(), errbuf));
        return -1;
    }

    prv_key = PEM_read_RSAPrivateKey(prvkey_file, NULL, NULL, NULL);
    if (prv_key == NULL) {
        printf("can't load prv key: %s\n", ERR_error_string(ERR_get_error(), errbuf));
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

    char errbuf[1024];

    /* read key */
    if ( read_rsa_keys() != 0 ) {
        printf("failed read_rsa_keys()\n");
        return -1;
    }

    // /* init req_data */
    // req_data_t *req_data;
	// req_data = (req_data_t*)malloc(sizeof(req_data_t));
    // uuid_generate(req_data->client_id);
    
    // /* nonce の作成 */
    // int ret;
    // ret = RAND_bytes(req_data->nonce, sizeof req_data->nonce);
    // unsigned long err = ERR_get_error();
    // if(ret != 1) {
    //     /* RAND_bytes failed */
    //     /* `err` is valid    */
    //     printf("RAND_bytes is failed\n");
    //     return 1;
    // }

    // /* int dec_req_data_t */
    // dec_req_data_t *dec_req_data;
    // dec_req_data = (dec_req_data_t*)malloc(sizeof(dec_req_data_t));
    // memcpy(&dec_req_data->req_data, req_data, sizeof(req_data_t));

    // // hashを計算して追加
    // ret = sha256_digest((const unsigned char*)&dec_req_data->req_data, sizeof(req_data_t), dec_req_data->digest);
    // // TODO 1なら success?
    // if (!ret) {
    //     printf("failed sha256_digest");
    //     return 1;
    // }

    unsigned char crypted_buf[2048];
    //crypted_buf = (unsigned char * )malloc(RSA_size(pub_key)); //TODO

    /* encrypt */
    char * enc_data = "its work!!its work!!its work!!its work!!its work!!its work!!its work!!";
    int enc_len;
    enc_len = RSA_public_encrypt(strlen(enc_data), (const unsigned char*)enc_data, crypted_buf, pub_key, RSA_PKCS1_PADDING);
    if ( enc_len == -1 ){
        printf("in encrypt: err=%s\n", ERR_error_string(ERR_get_error(), errbuf));
    }

    dec_req_data_t *ret_dec_req_data;
    ret_dec_req_data = (dec_req_data_t*)malloc(sizeof(dec_req_data_t));

    unsigned char * prv_key_buf = NULL;

    // 鍵の unsigned char を作る
	long prv_size = i2d_RSAPrivateKey(prv_key, &prv_key_buf);
    if(!prv_size){
        printf("PUB KEY TO DATA ZERO\n"); 
        return 1;
    }

    // int i;
	// for (i=0; i<prv_size; i++) {
	// 	printf("%x " ,prv_key[i]);
	// }
	// printf("\n------------\n");

    ecall_test(global_eid, (unsigned char*)enc_data, enc_len, ret_dec_req_data, prv_key_buf, prv_size);

    // // 復号化した構造体のhashを計算
    // unsigned char digest[32];
    // ret = sha256_digest((const unsigned char*)&ret_dec_req_data->req_data, sizeof(req_data_t), digest);
    // // TODO 1なら success?
    // if (!ret) {
    //     printf("failed sha256_digest\n");
    //     return 1;
    // }

    // // hashを比較
    // ret = memcmp(digest, ret_dec_req_data->digest, sizeof(digest));
    // if (!ret) {
    //     printf("hash is worng\n");
    //     return 1;
    // }

    // // 最初の構造体と同じになっているか比較
    // ret = memcmp(dec_req_data, ret_dec_req_data, sizeof(req_data_t));
    // if (!ret) {
    //     printf("failed memcmp\n");
    //     return 1;
    // }    

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