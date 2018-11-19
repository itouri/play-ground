#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// http://www.yamamotolab.jt.u-tokai.ac.jp/~hiroshi/education/crypto/pdf/rsaString.pdf
// http://www.trytoprogram.com/c-examples/c-program-to-encrypt-and-decrypt-string/

int is_prime(int pr)
{
    int i;
    long int j = sqrt(pr);
    for (i = 2; i <= j; i++)
    {
        if (pr % i == 0)
            return 0;
    }
    return 1;
}

// 何をしている関数?
unsigned long int cd(int a, int totient)
{
    long int k = 1;
    while (1)
    {
        k = k + totient;
        if (k % a == 0)
            return (k / a);
    }
}

void generate_key(int x, int y, int totient, unsigned long * dest_e, unsigned long * dest_d) {
    int i;
    int k = 0;
    int flag; //????????
    unsigned long e[99], d[99];

    for (i=2; i < totient; i++) {
        if (totient % i == 0)
            continue;

        if ( is_prime(i) && i != x && i != y ) {
            e[k] = i;
            flag = cd(i, totient);
            if ( flag > 0 ) {
                d[k] = flag;
                k++;
            }
            // 99ってなに???????
            if (k == 99)
                break;
        } 
    }
    *dest_e = e[0];
    *dest_d = d[0];
}

// return enc_msg size
void encrypt(unsigned char * enc_msg, unsigned char * msg, size_t msg_size, unsigned long pubkey, int n)
{
    int i, j;
    unsigned long k;

    for ( i=0; i < msg_size; i++ ) {
        k = 1;
        for (j=0; j < pubkey; j++) {
            k *= msg[i];
            k %= n;
        }
        enc_msg[i] = k;
    }

    printf("\nTHE ENCRYPTED MESSAGE IS\n");
    for (i = 0; i < msg_size; i++)
        printf("%02x ", enc_msg[i]);
    printf("\n");
}

void decrypt(unsigned char * msg, unsigned char * enc_msg, size_t msg_size, unsigned long prvkey, int n)
{
    int i, j;
    unsigned long k;

    for ( i=0; i < msg_size; i++ ) {
        k = 1;
        for (j=0; j < prvkey; j++) {
            k *= enc_msg[i];
            k %= n;
        }
        msg[i] = k;
    }

    printf("\nTHE DECRYPTED MESSAGE IS\n");
    for (i = 0; i < msg_size; i++)
        printf("%02x ", msg[i]);
    printf("\n");
}

int main()
{
    int i;
    int x, y, n, totient;
    unsigned char msg[100];
    int msg_len;
    unsigned long pubkey, prvkey;
    unsigned char * enc_msg;

    printf("ENTER FIRST PRIME NUMBER\n");
    scanf("%d", &x);
    if ( !is_prime(x) ) {
        printf("input is not prime number\n");
        return -1;
    }

    printf("ENTER SECOND PRIME NUMBER\n");
    scanf("%d", &y);
    if ( !is_prime(y) ) {
        printf("input is not prime number\n");
        return -1;
    }

    if ( x == y ) {
        printf("x and y are same\n");
        return -1;
    }

    // n = 253;
    // pubkey = 3;
    // prvkey = 147;

    printf("\nENTER MESSAGE OR STRING TO ENCRYPT\n");
    memset(msg, 0, sizeof(msg));
    scanf("%s", msg);

    printf("\nINPUTED MESSAGE\n");
    for (i = 0; i < strlen((const char*)msg); i++)
        printf("%02x ", msg[i]);
    printf("\n");

    msg_len = strlen((const char *)msg);
    enc_msg = (unsigned char*)malloc(msg_len);

    n = x * y;
    totient = (x - 1) * (y - 1);

    generate_key(x, y, totient, &pubkey, &prvkey);

    printf("n  : %d\n", n);
    printf("pub: %ld\n", pubkey);
    printf("prv: %ld\n", prvkey);

    encrypt(enc_msg, msg, msg_len, pubkey, n);
    decrypt(msg, enc_msg, msg_len, prvkey, n);

    return 0;
}