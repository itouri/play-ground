#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

void print_hex(unsigned char * str, int size) {
    int i=0;
    for (i=0; i < size; i++) {
        printf("%x ", str[i]);
    }
    printf("\n");
}

int main() 
{
    int crypted_len;
    int decrypted_len;
    char errbuf[1024];
    RSA *pub_key;
    RSA *prv_key;

    /* key load */
    const char *pub_file_path = "rsa-key-pub.pem";
    const char *prv_file_path = "rsa-key.pem";

    FILE *pubkey_file;
    FILE *prvkey_file;

    pubkey_file = fopen(pub_file_path, "r");
    if (pubkey_file == NULL) {
      printf("can't load pub key file: %s\n", ERR_error_string(ERR_get_error(), errbuf));
      exit(-1);
    }

    prvkey_file = fopen(prv_file_path, "r");
    if (prvkey_file == NULL) {
      printf("can't load prv key file: %s\n", ERR_error_string(ERR_get_error(), errbuf));
      exit(-1);
    }

    pub_key = PEM_read_RSAPublicKey(pubkey_file, NULL, NULL, NULL);
    if (pub_key == NULL) {
        printf("can't load pub key: %s\n", ERR_error_string(ERR_get_error(), errbuf));
        exit(-1);
    }

    prv_key = PEM_read_RSAPrivateKey(prvkey_file, NULL, NULL, NULL);
    if (prv_key == NULL) {
        printf("can't load prv key: %s\n", ERR_error_string(ERR_get_error(), errbuf));
        exit(-1);
    }

    unsigned char * prv_key_buf = NULL;

    // 鍵の unsigned char を作る
	long prv_size = i2d_RSAPrivateKey(prv_key, &prv_key_buf);
    if(!prv_size){
        printf("PUB KEY TO DATA ZERO\n"); 
        return 1;
    }

    RSA * dprv_key;
    dprv_key = d2i_RSAPrivateKey(NULL, (const unsigned char**)&prv_key_buf, prv_size);
	if( dprv_key == NULL){
        printf("failed d2i_RSAPrivateKey :%s\n", ERR_error_string(ERR_get_error(), errbuf));
        return 1;
    }

    const char * plain_text = "plain_text";
    unsigned char * crypted_buf;
    crypted_buf = malloc(RSA_size(pub_key)); //TODO

    unsigned char * decrypted_buf;
    decrypted_buf = malloc( strlen(plain_text) + 100 ); 

    crypted_len = RSA_public_encrypt(strlen(plain_text), (const unsigned char *)plain_text, crypted_buf, pub_key, RSA_PKCS1_PADDING);
    if ( crypted_len == -1 ){
        printf("in encrypt: err=%s\n", ERR_error_string(ERR_get_error(), errbuf));
    }

    decrypted_len = RSA_private_decrypt(crypted_len, crypted_buf, decrypted_buf, dprv_key, RSA_PKCS1_PADDING);
    if ( decrypted_len == -1 ){
        printf("in decrypt: err=%s\n", ERR_error_string(ERR_get_error(), errbuf));
    }

    //print_hex(decrypted_buf, decrypted_len);
    printf("result: %s\n", decrypted_buf);
    return 0;
}
