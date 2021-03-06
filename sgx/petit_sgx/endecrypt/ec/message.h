#ifndef __MESSAGE_H
#define __MESSAGE_H

#include <uuid/uuid.h>

#define DIGEST_SIZE 32
#define NONCE_SIZE 16

// grapheneSGXの mrenclave取得をするときに使う構造体
typedef struct req_data {
	unsigned char nonce[NONCE_SIZE];
	uuid_t client_id;
} req_data_t;

// master enclaveが受け取る構造体
typedef struct dec_req_data {
	unsigned char digest[DIGEST_SIZE];
	req_data_t req_data;
} dec_req_data_t;

// enclave が sp に完了報告するメッセージ digest はいらない
typedef struct msg_complete_struct {
	unsigned char nonce[NONCE_SIZE];
	uuid_t image_id;
	uuid_t client_id;
} msg_cmpt_t;

#endif