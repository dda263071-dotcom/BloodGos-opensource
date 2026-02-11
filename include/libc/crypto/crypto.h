#ifndef CRYPTO_H
#define CRYPTO_H

#include <libc/types.h>

/* ========== Bit rotation ========== */
uint32_t rotl32(uint32_t x, int n);
uint32_t rotr32(uint32_t x, int n);
uint64_t rotl64(uint64_t x, int n);
uint64_t rotr64(uint64_t x, int n);

/* ========== CRC32 ========== */
uint32_t crc32(const void* data, size_t len, uint32_t crc);

/* ========== Base64 ========== */
void base64_encode(const uint8_t* input, size_t len, char* output);
size_t base64_decode(const char* input, uint8_t* output);

/* ========== AES-128 ========== */
void aes128_encrypt(const uint8_t key[16], const uint8_t plaintext[16], uint8_t ciphertext[16]);
void aes128_decrypt(const uint8_t key[16], const uint8_t ciphertext[16], uint8_t plaintext[16]);

/* ========== SHA-256 ========== */
void sha256(const uint8_t* data, size_t len, uint8_t hash[32]);

#endif
