#include <string.h>

#include <uuid/uuid.h>
#include <openssl/evp.h>
#include <openssl/engine.h>
#include "message.h"

unsigned char* decrypt(
    const char* key,
    const unsigned char* data,
    const size_t datalen,
    const unsigned char* iv,
    unsigned char* dest,
    const size_t destlen
){
	EVP_CIPHER_CTX *de;
    // え？enclaveでなんで malloc できるん？
    de = (EVP_CIPHER_CTX *)malloc(sizeof(EVP_CIPHER_CTX));
    //memsetって中で何してるん？
    //memset(de, 0, sizeof(EVP_CIPHER_CTX));

	int f_len = 0;
	int p_len = datalen;

	memset(dest, 0x00, destlen);

	EVP_CIPHER_CTX_init(de);
	EVP_DecryptInit_ex(de, EVP_aes_128_cbc(), NULL, (unsigned char*)key, iv);

	EVP_DecryptUpdate(de, (unsigned char *)dest, &p_len, data, datalen);
	//EVP_DecryptFinal_ex(&de, (unsigned char *)(dest + p_len), &f_len);

	EVP_CIPHER_CTX_cleanup(de);

	return dest;
}