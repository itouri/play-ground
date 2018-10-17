#ifndef __CRYPTO_H
#define __CRYPTO_H

#include <sgx_utils.h>
#include <sgx_key_exchange.h>

sgx_status_t sgx_rijndael128GCM_encrypt(
    const sgx_aes_gcm_128bit_key_t *p_key, 
    const uint8_t *p_src,
    uint32_t src_len,
    uint8_t *p_dst,
    const uint8_t *p_iv,
    uint32_t iv_len,
    const uint8_t *p_aad,
    uint32_t aad_len,
    sgx_aes_gcm_128bit_tag_t *p_out_mac);


sgx_status_t sgx_rijndael128GCM_decrypt(
    const sgx_aes_gcm_128bit_key_t *p_key,
    const uint8_t *p_src,
    uint32_t src_len,
    uint8_t *p_dst,
    const uint8_t *p_iv,
    uint32_t iv_len,
    const uint8_t *p_aad,
    uint32_t aad_len,
    const sgx_aes_gcm_128bit_tag_t *p_in_mac);

#endif