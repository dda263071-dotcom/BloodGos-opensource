#include <libc/math/math.h>

/* Integer square root (Newton-Raphson) */
uint32_t isqrt(uint32_t x) {
    if (x == 0) return 0;
    uint32_t guess = x;
    while (1) {
        uint32_t better = (guess + x / guess) >> 1;
        if (better >= guess) break;
        guess = better;
    }
    return guess;
}

/* Fixed-point square root (16.16) */
fixed_t fixed_sqrt(fixed_t x) {
    if (x <= 0) return 0;
    /* Shift right 2 bits -> x / 4, lalu isqrt, lalu shift left */
    /* Metode: isqrt(x * 2^16) = isqrt(x) * 2^8, tapi x dalam fixed-point */
    uint32_t val = (uint32_t)x;
    uint32_t root = isqrt(val << 16);  /* perlu 64-bit? val << 16 bisa overflow */
    /* Lebih aman pake uint64_t */
    uint64_t v = (uint64_t)x << 16;
    uint32_t r = (uint32_t)isqrt((uint32_t)(v >> 32 ? 0xFFFFFFFF : v));
    return (fixed_t)r;
}

/* Atau pakai metode binary search */
fixed_t fixed_sqrt_binary(fixed_t x) {
    if (x <= 0) return 0;
    fixed_t low = 0, high = (x < FIXED_ONE) ? FIXED_ONE : x;
    while (low <= high) {
        fixed_t mid = (low + high) >> 1;
        fixed_t sq = fixed_mul(mid, mid);
        if (sq == x) return mid;
        if (sq < x) low = mid + 1;
        else high = mid - 1;
    }
    return high;
}
