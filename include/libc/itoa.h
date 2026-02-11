#ifndef ITOA_H
#define ITOA_H

#include <libc/types.h>
#include <libc/string/string.h>   /* strlen now here */

void itoa(int32_t value, char* str, int base);
void utoa(uint32_t value, char* str, int base);
void itoa64(int64_t value, char* str, int base);
void utoa64(uint64_t value, char* str, int base);
void reverse_string(char* str, int length);

#endif
