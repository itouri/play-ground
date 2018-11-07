#ifndef METADATA_TYPES_H_
#define METADATA_TYPES_H_

#include <stdint.h>
#include "sgx_report.h"

#define CLIENT_ID_SIZE 16
#define HMAC_SIZE 32

typedef struct _image_metadata_t {
    uint8_t client_id[CLIENT_ID_SIZE];
    sgx_measurement_t mrenclave;
    uint8_t hmac[HMAC_SIZE];
} image_metadata_t;

typedef struct _create_req_metadata_t {
    uint8_t client_id[CLIENT_ID_SIZE];
    uint8_t nonce[32];
    uint8_t hmac[HMAC_SIZE];
} create_req_metadata_t;

#endif