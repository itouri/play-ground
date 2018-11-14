#ifndef __DATATYPES_H
#define __DATATYPES_H

#include <stdlib.h>

typedef uint8_t image_id_t[16];

typedef struct _la_arg_t {
    uint8_t * imd;
    size_t imd_sz;
    uint8_t * crm;
    size_t crm_sz;
} la_arg_t;

typedef struct _la_server_arg_t {
    unsigned char * socket_path;
    la_arg_t arg;
} la_server_arg_t;

#endif