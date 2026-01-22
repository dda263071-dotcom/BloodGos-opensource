/**************************************************************
 * VGA Text Mode Header - BloodG OS
 * Complete VGA driver interface
 **************************************************************/

#ifndef _VGA_H
#define _VGA_H

#include <stdint.h>
#include <stddef.h>

/* ==================== VGA CONSTANTS ==================== */

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  ((uint16_t*)0xB8000)

/* ==================== VGA COLORS ==================== */

typedef enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_YELLOW = 14,
    VGA_COLOR_WHITE = 15,
} vga_color_t;

/* ==================== VGA FUNCTIONS ==================== */

/**
 * Initialize VGA text mode terminal
 */
void vga_init(void);

/**
 * Clear terminal screen
 */
void vga_clear(void);

/**
 * Write character to terminal
 * @param c Character to write
 */
void vga_putchar(char c);

/**
 * Write string to terminal
 * @param str String to write
 */
void vga_puts(const char* str);

/**
 * Set terminal color
 * @param fg Foreground color
 * @param bg Background color
 */
void vga_set_color(uint8_t fg, uint8_t bg);

/**
 * Get current terminal color
 * @return Current color byte
 */
uint8_t vga_get_color(void);

/**
 * Set cursor position
 * @param x Column (0-79)
 * @param y Row (0-24)
 */
void vga_set_cursor(size_t x, size_t y);

/**
 * Get cursor position
 * @param x Output: column
 * @param y Output: row
 */
void vga_get_cursor(size_t* x, size_t* y);

/**
 * Enable hardware cursor
 * @param cursor_start Starting scanline (0-15)
 * @param cursor_end Ending scanline (0-15)
 */
void vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);

/**
 * Disable hardware cursor
 */
void vga_disable_cursor(void);

/**
 * Scroll terminal up by one line
 */
void vga_scroll(void);

/**
 * Write character at specific position
 * @param c Character to write
 * @param color Color to use
 * @param x Column
 * @param y Row
 */
void vga_putchar_at(char c, uint8_t color, size_t x, size_t y);

/**
 * Clear line from cursor to end
 */
void vga_clear_eol(void);

/**
 * Clear entire line
 * @param y Line number (0-24)
 */
void vga_clear_line(size_t y);

/**
 * Print colored text
 * @param str String to print
 * @param fg Foreground color
 * @param bg Background color
 */
void vga_print_color(const char* str, uint8_t fg, uint8_t bg);

/**
 * Simple formatted output (basic implementation)
 * @param format Format string
 * @param ... Arguments
 */
void vga_printf(const char* format, ...);

/* ==================== UTILITY FUNCTIONS ==================== */

/**
 * Create VGA color byte from foreground and background
 * @param fg Foreground color
 * @param bg Background color
 * @return Color byte
 */
static inline uint8_t vga_make_color(vga_color_t fg, vga_color_t bg) {
    return fg | (bg << 4);
}

/**
 * Create VGA character entry
 * @param c Character
 * @param color Color byte
 * @return VGA entry
 */
static inline uint16_t vga_make_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

#endif /* _VGA_H */
