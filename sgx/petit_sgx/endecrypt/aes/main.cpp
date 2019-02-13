#include <stdio.h>

#include "encrypt.h"
#include "decrypt.h"
#include "message.h"

void print_imd(imd_t imd) {
    char out[36+1];
    uuid_unparse(imd.cliend_id, out);
    printf("client_id: %s\n",out);
    printf("mr_enclave: ");
    int i;
    for (i=0; i<32; i++) {
        printf("%u", imd.mr_enclave.m[i]);
    }
    printf("\n");
}

int main (void)
{
    imd_t imd;
    char imd_key[16] = "abxsabxsabxsabx";
    unsigned char *iv = {0}; // 16Byte

    // from_file -> 

    uuid_generate(imd.cliend_id);
    uint8_t mr[32] = {0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7};
    memcpy(&imd.mr_enclave, mr, sizeof(uint8_t)*32);

    printf("\n--- before encrypt ---\n");
    print_imd(imd);

    char out[36+1];
    uuid_unparse(imd.cliend_id, out);
    unsigned char cry_imd[32]; //なんで32?

    encrypt(imd_key, (unsigned char*)&imd, sizeof(imd_t), iv, cry_imd, sizeof(cry_imd));

    printf("\n--- after encrypt ---\n");
    printf("cry_imd: %s\n", cry_imd);

    imd_t re_imd;
    decrypt(imd_key, cry_imd, sizeof(unsigned char)*32, iv, (unsigned char*)&re_imd, sizeof(imd_t));

    printf("\n--- decrypt ---\n");
    print_imd(imd);

    return 0;
}
