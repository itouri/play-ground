#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/engine.h>
#include <string.h>
#include "message.h"

// MRENCLAVE を rsa 公開鍵で暗号化するために使う
unsigned char* encrypt( const char* key,
						const unsigned char* data,
						const size_t datalen,
						const unsigned char* iv,
						unsigned char* dest,
						const size_t destlen)
{
	EVP_CIPHER_CTX *en;
    // やっぱmallocが必要だよね
    en = (EVP_CIPHER_CTX *)malloc(sizeof(EVP_CIPHER_CTX));
    EVP_CIPHER_CTX_init(en);

	int i, f_len=0;
	int c_len = destlen;

	memset(dest, 0x00, destlen);

	EVP_EncryptInit_ex(en, EVP_aes_128_cbc(), NULL, (unsigned char*)key, iv);

	EVP_EncryptUpdate(en, dest, &c_len, (unsigned char *)data, datalen);
	//EVP_EncryptFinal_ex(&en, (unsigned char *)(dest + c_len), &f_len);

	EVP_CIPHER_CTX_cleanup(en);

	return dest;
}
