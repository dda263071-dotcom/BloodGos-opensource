#ifndef MATH_H
#define MATH_H

#include <libc/types.h>   // relatif dari -Iinclude

/* ========== Fixed-point 16.16 ========== */
typedef int32_t fixed_t;

#define FIXED_SHIFT     16
#define FIXED_SCALE     (1 << FIXED_SHIFT)
#define FIXED_ONE       (1 << FIXED_SHIFT)
#define FIXED_HALF      (1 << (FIXED_SHIFT - 1))

/* Konversi */
fixed_t fixed_from_int(int x);
fixed_t fixed_from_float(float f);   /* opsional, butuh soft-float */
int     fixed_to_int(fixed_t x);
float   fixed_to_float(fixed_t x);

/* Aritmatika fixed-point */
fixed_t fixed_add(fixed_t a, fixed_t b);
fixed_t fixed_sub(fixed_t a, fixed_t b);
fixed_t fixed_mul(fixed_t a, fixed_t b);
fixed_t fixed_div(fixed_t a, fixed_t b);

/* ========== Trigonometri (sudut dalam radian fixed-point) ========== */
fixed_t fixed_sin(fixed_t rad);
fixed_t fixed_cos(fixed_t rad);
fixed_t fixed_tan(fixed_t rad);

/* ========== Akar kuadrat ========== */
fixed_t fixed_sqrt(fixed_t x);        /* fixed-point square root */
uint32_t isqrt(uint32_t x);           /* integer square root */

/* ========== Nilai absolut ========== */
fixed_t fixed_abs(fixed_t x);

#endif
