#include "print_string.h"
#include "io.h"

#define VIDEO_MEMORY 0xB8000
#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25

static uint16_t* video_mem = (uint16_t*)VIDEO_MEMORY;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;
static uint8_t color = (WHITE << 4) | BLACK;

static void scroll(void) {
    for (int y = 0; y < SCREEN_HEIGHT - 1; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
            video_mem[y * SCREEN_WIDTH + x] = video_mem[(y + 1) * SCREEN_WIDTH + x];
    for (int x = 0; x < SCREEN_WIDTH; x++)
        video_mem[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + x] = ' ' | (color << 8);
}

static void update_cursor(void) {
    uint16_t pos = cursor_y * SCREEN_WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void print_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 4) & ~(4 - 1);
    } else {
        video_mem[cursor_y * SCREEN_WIDTH + cursor_x] = c | (color << 8);
        cursor_x++;
    }

    if (cursor_x >= SCREEN_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= SCREEN_HEIGHT) {
        scroll();
        cursor_y = SCREEN_HEIGHT - 1;
    }
    update_cursor();
}

void print_string(const char* str) {
    while (*str) print_char(*str++);
}

void print_string_at(const char* str, uint8_t x, uint8_t y) {
    uint8_t old_x = cursor_x, old_y = cursor_y;
    cursor_x = x; cursor_y = y;
    print_string(str);
    cursor_x = old_x; cursor_y = old_y;
    update_cursor();
}

void clear_screen(void) {
    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
            video_mem[y * SCREEN_WIDTH + x] = ' ' | (color << 8);
    cursor_x = 0; cursor_y = 0;
    update_cursor();
}

void set_cursor_position(uint8_t x, uint8_t y) {
    cursor_x = x; cursor_y = y;
    if (cursor_x >= SCREEN_WIDTH) cursor_x = SCREEN_WIDTH - 1;
    if (cursor_y >= SCREEN_HEIGHT) cursor_y = SCREEN_HEIGHT - 1;
    update_cursor();
}
