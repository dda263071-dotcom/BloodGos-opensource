#include <libc/math/math.h>

/* Tabel sinus untuk 0..90 derajat (0 .. PI/2) dengan fixed-point 16.16 */
/* Nilai = sin(angle) * 65536, angle dalam derajat * 256 (agar presisi) */
static const uint32_t sin_table_deg[91] = {
    0,     1144,  2287,  3429,  4570,  5711,  6850,  7987,  9121,  10252, 
    11380, 12505, 13626, 14742, 15855, 16962, 18064, 19161, 20252, 21336, 
    22414, 23486, 24550, 25607, 26656, 27697, 28729, 29753, 30767, 31772, 
    32768, 33754, 34729, 35693, 36647, 37590, 38521, 39441, 40348, 41243, 
    42126, 42995, 43852, 44695, 45525, 46341, 47143, 47930, 48703, 49461, 
    50203, 50931, 51643, 52339, 53020, 53684, 54332, 54963, 55578, 56175, 
    56756, 57319, 57865, 58393, 58903, 59396, 59870, 60326, 60764, 61183, 
    61584, 61966, 62328, 62672, 62997, 63303, 63589, 63856, 64104, 64332, 
    64540, 64729, 64898, 65048, 65177, 65287, 65376, 65446, 65496, 65526, 
    65536 
};

/* Konversi radian fixed-point ke derajat (0..360) */
static int rad_to_deg(fixed_t rad) {
    /* rad * (180 / PI) * 65536 -> (180 << 16)/PI */
    fixed_t deg_fixed = fixed_mul(rad, 0x6487E);  /* 180/PI = 57.29578, *65536 = 0x6487E */
    return (deg_fixed >> 16) % 360;
}

/* Sinus fixed-point, input radian 16.16 */
fixed_t fixed_sin(fixed_t rad) {
    int deg = rad_to_deg(rad);
    int sign = 1;
    
    /* Kuadran */
    deg %= 360;
    if (deg < 0) deg += 360;
    
    if (deg >= 180) {
        sign = -1;
        deg -= 180;
    }
    if (deg >= 90) {
        deg = 180 - deg;
    }
    
    uint32_t sin_val = sin_table_deg[deg];
    return sign * sin_val;
}

fixed_t fixed_cos(fixed_t rad) {
    /* cos(x) = sin(x + PI/2) */
    return fixed_sin(rad + (0x19220));  /* PI/2 = 1.5708 * 65536 = 0x19220 */
}

fixed_t fixed_tan(fixed_t rad) {
    fixed_t s = fixed_sin(rad);
    fixed_t c = fixed_cos(rad);
    if (c == 0) return 0x7FFFFFFF; /* infinite, return max */
    return fixed_div(s, c);
}
