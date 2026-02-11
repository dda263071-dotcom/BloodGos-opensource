#include <libc/crypto/crypto.h>

uint32_t rotl32(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

uint32_t rotr32(uint32_t x, int n) {
    return (x >> n) | (x << (32 - n));
}

uint64_t rotl64(uint64_t x, int n) {
    return (x << n) | (x >> (64 - n));
}

uint64_t rotr64(uint64_t x, int n) {
    return (x >> n) | (x << (64 - n));
}
