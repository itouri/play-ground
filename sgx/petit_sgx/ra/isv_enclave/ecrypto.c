#include <string.h>
#include <assert.h>

#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/obj_mac.h>
#include <openssl/evp.h>

#define DATA_LEN	20
// TODO EC_NAME とか EC_NID ってなんだろう
#define EC_NAME	"prime192v1"
#define EC_NID	NID_X9_62_prime192v1

typedef unsigned char byte;

static void
printHex(const char *title, const unsigned char *s, int len) 
{
	int    n;
	printf("%s:", title);
	for (n = 0; n < len; ++n) {
		if ((n % 16) == 0) {
			printf("\n%04x", n);
		}
		printf(" %02x", s[n]);
	}
	printf("\n");
}

/**
 * Encryption
 */
int
elgamal_encrypt(byte **encData, byte *data, int dataLen, const EC_KEY *eckey) 
{
	BN_CTX *ctx = NULL;
	BIGNUM *r = NULL, *p = NULL, *m;
	EC_POINT *C1 = NULL, *C2 = NULL;
	EC_POINT *Tmp = NULL, *M;
	const EC_POINT *Pkey;
	const EC_GROUP *group;
	int    c1Len, c2Len;
	int    rv;

	if ((group = EC_KEY_get0_group(eckey)) == NULL) {
		return 0;
	}
	p = BN_new();
	ctx = BN_CTX_new();
	EC_GROUP_get_curve_GFp(group, p, NULL, NULL, ctx);
#ifdef DEBUG
	printf(" p = ");
	BN_print_fp(stdout, p);
	puts("");
#endif

	/* C1 = r*G */
	C1 = EC_POINT_new(group);

	/* generate random number r */ 
	r = BN_new();
	M = EC_POINT_new(group);
	m = BN_new();
	do {
		if (!BN_rand_range(r, p)) {
			return 0;
		}
	} while (BN_is_zero(r));
#ifdef DEBUG
	printf(" r = ");
	BN_print_fp(stdout, r);
	puts("");
#endif

	EC_POINT_mul(group, C1, r, NULL, NULL, ctx);

	/* C2 = r*P + M */ 
	/* M */
	BN_bin2bn(data, dataLen, m);
	rv = EC_POINT_set_compressed_coordinates_GFp(group, M, m, 1, ctx);
	if (!rv) {
		return 0;
	}

	C2 = EC_POINT_new(group);
	Tmp = EC_POINT_new(group);
	Pkey = EC_KEY_get0_public_key(eckey);
	EC_POINT_mul(group, Tmp, NULL, Pkey, r, ctx);
	EC_POINT_add(group, C2, Tmp, M, ctx);

	/* cipher text C = (C1, C2) */ 
	c1Len = EC_POINT_point2oct(group, C1, POINT_CONVERSION_COMPRESSED,
							   NULL, 0, ctx);
#ifdef DEBUG
	printf(" Point converted length (C1) = %d\n", c1Len);
#endif
	c2Len =	EC_POINT_point2oct(group, C2, POINT_CONVERSION_COMPRESSED,
							   NULL, 0, ctx);
#ifdef DEBUG
	printf(" Point converted length (C2) = %d\n", c1Len);
#endif
	*encData = OPENSSL_malloc(c1Len + c2Len);
	EC_POINT_point2oct(group, C1, POINT_CONVERSION_COMPRESSED,
							*encData, c1Len, ctx);
	EC_POINT_point2oct(group, C2, POINT_CONVERSION_COMPRESSED,
							*encData + c1Len, c2Len, ctx);

	BN_clear_free(p);
	BN_clear_free(r);
	BN_clear_free(m);
	EC_POINT_free(C1);
	EC_POINT_free(C2);
	EC_POINT_free(M);
	EC_POINT_free(Tmp);
	BN_CTX_free(ctx);

	return (c1Len + c2Len);
}

/**
 * Decryption
 */
int
elgamal_decrypt(byte **decData, byte *encData, int encLen, const EC_KEY *eckey) 
{
	int rv;
	const EC_GROUP *group;
	const BIGNUM *prvKey;
	BN_CTX *ctx;
	EC_POINT *C1 = NULL, *C2 = NULL;
	EC_POINT *M = NULL, *Tmp = NULL;

	group = EC_KEY_get0_group(eckey);
	prvKey = EC_KEY_get0_private_key(eckey);
#ifdef DEBUG
	printf(" prvKey = ");
	BN_print_fp(stdout, prvKey);
	puts("");
#endif
	C1 = EC_POINT_new(group);
	C2 = EC_POINT_new(group);
	ctx = BN_CTX_new();

	/* C1 */
#ifdef DEBUG
	printHex("C1", encData, encLen / 2);
#endif
	rv = EC_POINT_oct2point(group, C1, encData, encLen / 2, ctx);
	if (!rv) {
		fprintf(stderr, "EC_POINT_oct2point error (C1)\n");
		return 0;
	}

	/* C2 */
#ifdef DEBUG
	printHex("C2", encData + encLen / 2, encLen / 2);
#endif
	rv = EC_POINT_oct2point(group, C2, encData + encLen / 2, encLen / 2,
							ctx);
	if (!rv) {
		fprintf(stderr, "EC_POINT_oct2point error (C2)\n");
		return 0;
	}
	Tmp = EC_POINT_new(group);
	M = EC_POINT_new(group);

	/* M = C2 - x C1 */ 
	EC_POINT_mul(group, Tmp, NULL, C1, prvKey, ctx);
	EC_POINT_invert(group, Tmp, ctx);
	EC_POINT_add(group, M, C2, Tmp, ctx);

	/* Output M */ 
	rv = EC_POINT_point2oct(group, M, POINT_CONVERSION_COMPRESSED, NULL, 0,
							ctx);

#ifdef DEBUG
	printf(" Point converted length = %d\n", rv);
#endif
	*decData = OPENSSL_malloc(rv);
	EC_POINT_point2oct(group, M, POINT_CONVERSION_COMPRESSED, *decData,
					   rv, ctx);

	EC_POINT_free(C1);
	EC_POINT_free(C2);
	EC_POINT_free(M);
	EC_POINT_free(Tmp);
	BN_CTX_free(ctx);

	return rv;
}