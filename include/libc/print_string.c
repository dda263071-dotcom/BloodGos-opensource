#ifndef PRINT_STRING_H
#define PRINT_STRING_H

#include "types.h"

void print_char(char c);
void print_string(const char* str);
void print_string_at(const char* str, uint8_t x, uint8_t y);
void clear_screen(void);
void set_cursor_position(uint8_t x, uint8_t y);

enum vga_color {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GREY = 7,
    DARK_GREY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    YELLOW = 14,
    WHITE = 15
};

#endif
