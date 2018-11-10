#include<sgx_utils.h>

#ifdef __linux__
/*
 * __memset_vp is a volatile pointer to a function.
 * It is initialised to point to memset, and should never be changed.
 */
static void * (* const volatile __memset_vp)(void *, int, size_t)
    = (memset);

#undef memset_s /* in case it was defined as a macro */

extern "C" int memset_s(void *s, size_t smax, int c, size_t n)
{
    int err = 0;

    if (s == NULL) {
        err = EINVAL;
        goto out;
    }

    if (n > smax) {
        err = EOVERFLOW;
        n = smax;
    }

    /* Calling through a volatile pointer should never be optimised away. */
    (*__memset_vp)(s, c, n);

    out:
    if (err == 0)
        return 0;
    else {
        errno = err;
        /* XXX call runtime-constraint handler */
        return err;
    }
}
#endif

sgx_status_t sgx_rijndael128GCM_encrypt(const sgx_aes_gcm_128bit_key_t *p_key, const uint8_t *p_src, uint32_t src_len,
                                        uint8_t *p_dst, const uint8_t *p_iv, uint32_t iv_len, const uint8_t *p_aad, uint32_t aad_len,
                                        sgx_aes_gcm_128bit_tag_t *p_out_mac)
{
	if ((src_len > INT_MAX) || (aad_len > INT_MAX) || (p_key == NULL) || ((src_len > 0) && (p_dst == NULL)) || ((src_len > 0) && (p_src == NULL))
		|| (p_out_mac == NULL) || (iv_len != SGX_AESGCM_IV_SIZE) || ((aad_len > 0) && (p_aad == NULL))
		|| (p_iv == NULL) || ((p_src == NULL) && (p_aad == NULL)))
	{
		return SGX_ERROR_INVALID_PARAMETER;
	}
	sgx_status_t ret = SGX_ERROR_UNEXPECTED;
	int len = 0;
	EVP_CIPHER_CTX * pState = NULL;

	//CLEAR_OPENSSL_ERROR_QUEUE;

	do {
		// Create and init ctx
		//
		if (!(pState = EVP_CIPHER_CTX_new())) {
			ret = SGX_ERROR_OUT_OF_MEMORY;
			break;
		}

		// Initialise encrypt, key and IV
		//
		if (1 != EVP_EncryptInit_ex(pState, EVP_aes_128_gcm(), NULL, (unsigned char*)p_key, p_iv)) {
			break;
		}

		// Provide AAD data if exist
		//
		if (NULL != p_aad) {
			if (1 != EVP_EncryptUpdate(pState, NULL, &len, p_aad, aad_len)) {
				break;
			}
		}

		// Provide the message to be encrypted, and obtain the encrypted output.
		//
		if (1 != EVP_EncryptUpdate(pState, p_dst, &len, p_src, src_len)) {
			break;
		}

		// Finalise the encryption
		//
		if (1 != EVP_EncryptFinal_ex(pState, p_dst + len, &len)) {
			break;
		}

		// Get tag
		//
		if (1 != EVP_CIPHER_CTX_ctrl(pState, EVP_CTRL_GCM_GET_TAG, SGX_AESGCM_MAC_SIZE, p_out_mac)) {
			break;
		}
		ret = SGX_SUCCESS;
	} while (0);

	if (ret != SGX_SUCCESS) {
        //GET_LAST_OPENSSL_ERROR;
	}

	// Clean up and return
	//
	if (pState) {
			EVP_CIPHER_CTX_free(pState);
	}
	return ret;
}

sgx_status_t sgx_rijndael128GCM_decrypt(const sgx_aes_gcm_128bit_key_t *p_key, const uint8_t *p_src,
                                        uint32_t src_len, uint8_t *p_dst, const uint8_t *p_iv, uint32_t iv_len,
                                        const uint8_t *p_aad, uint32_t aad_len, const sgx_aes_gcm_128bit_tag_t *p_in_mac)
{
	uint8_t l_tag[SGX_AESGCM_MAC_SIZE];

	if ((src_len > INT_MAX) || (aad_len > INT_MAX) || (p_key == NULL) || ((src_len > 0) && (p_dst == NULL)) || ((src_len > 0) && (p_src == NULL))
		|| (p_in_mac == NULL) || (iv_len != SGX_AESGCM_IV_SIZE) || ((aad_len > 0) && (p_aad == NULL))
		|| (p_iv == NULL) || ((p_src == NULL) && (p_aad == NULL)))
	{
		return SGX_ERROR_INVALID_PARAMETER;
	}
	int len = 0;
	sgx_status_t ret = SGX_ERROR_UNEXPECTED;
	EVP_CIPHER_CTX * pState = NULL;

	// CLEAR_OPENSSL_ERROR_QUEUE; ???

	// Autenthication Tag returned by Decrypt to be compared with Tag created during seal
	//
	memset_s(&l_tag, SGX_AESGCM_MAC_SIZE, 0, SGX_AESGCM_MAC_SIZE);
	memcpy(l_tag, p_in_mac, SGX_AESGCM_MAC_SIZE);

	do {
		// Create and initialise the context
		//
		if (!(pState = EVP_CIPHER_CTX_new())) {
			ret = SGX_ERROR_OUT_OF_MEMORY;
			break;
		}

		// Initialise decrypt, key and IV
		//
		if (!EVP_DecryptInit_ex(pState, EVP_aes_128_gcm(), NULL, (unsigned char*)p_key, p_iv)) {
			break;
		}

		// Provide AAD data if exist
		//
		if (NULL != p_aad) {
			if (!EVP_DecryptUpdate(pState, NULL, &len, p_aad, aad_len)) {
				break;
			}
		}

		// Decrypt message, obtain the plaintext output
		//
		if (!EVP_DecryptUpdate(pState, p_dst, &len, p_src, src_len)) {
			break;
		}

		// Update expected tag value
		//
		if (!EVP_CIPHER_CTX_ctrl(pState, EVP_CTRL_GCM_SET_TAG, SGX_AESGCM_MAC_SIZE, l_tag)) {
			break;
		}

		// Finalise the decryption. A positive return value indicates success,
		// anything else is a failure - the plaintext is not trustworthy.
		//
		if (EVP_DecryptFinal_ex(pState, p_dst + len, &len) <= 0) {
			break;
		}
		ret = SGX_SUCCESS;
	} while (0);

	if (ret != SGX_SUCCESS) {
		//GET_LAST_OPENSSL_ERROR; ???
	}

	// Clean up and return
	//
	if (pState != NULL) {
		EVP_CIPHER_CTX_free(pState);
	}
	memset_s(&l_tag, SGX_AESGCM_MAC_SIZE, 0, SGX_AESGCM_MAC_SIZE);
	return ret;
}