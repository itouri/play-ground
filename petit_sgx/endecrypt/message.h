#ifndef __MESSAGE_H
#define __MESSAGE_H

#include "/Users/akiyama/Documents/git/linux-sgx/common/inc/sgx_report.h"
#include <uuid/uuid.h>

#define MAC_SIZE 16

typedef struct image_meta_data_struct {
	unsigned char mac[MAC_SIZE];
	sgx_measurement_t mr_enclave;
	uuid_t cliend_id;
} imd_t;

typedef struct msg_complete_struct {
	uuid_t image_id;
	uuid_t cliend_id;
} msg_cmpt_t;

#endif