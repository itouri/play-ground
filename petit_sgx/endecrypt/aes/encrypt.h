#ifndef __ENCRYPT_H
#define __ENCRYPT_H

#include <string.h>
#include <uuid/uuid.h>

unsigned char* encrypt( const char* key,
						const unsigned char* data,
						const size_t datalen,
						const unsigned char* iv,
						unsigned char* dest,
						const size_t destlen);

#endif
