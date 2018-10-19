#ifndef __ECEYPTO_H
#define __ECRYPTO_H

#include <openssl/ec.h>

typedef unsigned char byte;
int intelgamal_encrypt(byte **encData, byte *data, int dataLen, const EC_KEY *eckey);
int elgamal_decrypt(byte **decData, byte *encData, int encLen, const EC_KEY *eckey);

#endif