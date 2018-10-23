#ifndef __UCRYPTO_H
#define __UCRYPTO_H

#include <openssl/ec.h>

int elgamal_encrypt(unsigned char **encData, unsigned char *data, int dataLen, const EC_KEY *eckey) ;
int elgamal_decrypt(unsigned char **decData, unsigned char *encData, int encLen, const EC_KEY *eckey);

#endif