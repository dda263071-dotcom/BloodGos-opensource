#include "stdio.h"
#include "itoa.h"
#include <stdarg.h>   // compiler tetap punya stdarg.h meski freestanding

static void print_pad(char pad_char, int width, int left_align) {
    // fungsi internal untuk padding
}

int printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    // ... parsing format, panggil itoa/utoa/print_string
    va_end(args);
    return 0;
}
