#ifndef STDIO_H
#define STDIO_H

#include "types.h"
#include "print_string.h"   // untuk print_char dll

int  putchar(int c);
int  puts(const char* s);
int  printf(const char* format, ...);
int  sprintf(char* buffer, const char* format, ...);
int  snprintf(char* buffer, size_t n, const char* format, ...);

#endif
