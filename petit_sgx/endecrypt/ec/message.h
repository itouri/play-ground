#ifndef __MESSAGE_H
#define __MESSAGE_H

#include <uuid/uuid.h>

#define MAC_SIZE 16

// master enclaveが受け取る構造体
typedef struct req_data {
	unsigned char mac[MAC_SIZE];
	unsigned char nonce[16];
	uuid_t client_id;
} req_data_t;

// grapheneSGXの mrenclave取得をするときに使う構造体
typedef struct ra_req_data {
	unsigned char nonce[16];
	uuid_t client_id;
} ra_req_data_t;

typedef struct msg_complete_struct {
	uuid_t image_id;
	uuid_t client_id;
} msg_cmpt_t;

#endif