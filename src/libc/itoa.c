#include "itoa.h"

void reverse_string(char* str, int length) {
    int start = 0, end = length - 1;
    while (start < end) {
        char tmp = str[start];
        str[start] = str[end];
        str[end] = tmp;
        start++;
        end--;
    }
}

int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

void itoa(int32_t value, char* str, int base) {
    int i = 0;
    int is_negative = 0;
    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    if (value < 0 && base == 10) {
        is_negative = 1;
        value = -value;
    }
    while (value != 0) {
        int rem = value % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value /= base;
    }
    if (is_negative) str[i++] = '-';
    str[i] = '\0';
    reverse_string(str, i);
}

void utoa(uint32_t value, char* str, int base) {
    int i = 0;
    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    while (value != 0) {
        int rem = value % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value /= base;
    }
    str[i] = '\0';
    reverse_string(str, i);
}

void itoa64(int64_t value, char* str, int base) {
    int i = 0;
    int is_negative = 0;
    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    if (value < 0 && base == 10) {
        is_negative = 1;
        value = -value;
    }
    while (value != 0) {
        int64_t rem = value % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value /= base;
    }
    if (is_negative) str[i++] = '-';
    str[i] = '\0';
    reverse_string(str, i);
}

void utoa64(uint64_t value, char* str, int base) {
    int i = 0;
    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    while (value != 0) {
        uint64_t rem = value % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value /= base;
    }
    str[i] = '\0';
    reverse_string(str, i);
}
