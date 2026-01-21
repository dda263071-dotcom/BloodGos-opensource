/**************************************************************
 * VGA Text Mode Driver - BloodG OS
 * Complete VGA driver with color support
 **************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "io.h"
#include "string.h"

// VGA memory and dimensions
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  ((uint16_t*)0xB8000)

// Terminal state
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

// Create VGA entry from character and color
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t)uc | (uint16_t)color << 8;
}

// Create color byte from foreground and background
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

// Initialize terminal
void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    terminal_buffer = VGA_MEMORY;
    
    // Clear screen
    terminal_clear();
    
    // Enable cursor
    terminal_enable_cursor(0, 15);
    
    print_string("VGA: Text mode initialized (80x25)\n");
}

// Clear terminal
void terminal_clear(void) {
    uint16_t blank = vga_entry(' ', terminal_color);
    
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = blank;
        }
    }
    
    terminal_row = 0;
    terminal_column = 0;
    terminal_update_cursor();
}

// Put character at current position
void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_newline();
        return;
    }
    
    if (c == '\b') {
        terminal_backspace();
        return;
    }
    
    if (c == '\r') {
        terminal_column = 0;
        terminal_update_cursor();
        return;
    }
    
    if (c == '\t') {
        terminal_column = (terminal_column + 8) & ~7;
        if (terminal_column >= VGA_WIDTH) {
            terminal_newline();
        }
        terminal_update_cursor();
        return;
    }
    
    // Printable character
    terminal_buffer[terminal_row * VGA_WIDTH + terminal_column] = 
        vga_entry(c, terminal_color);
    
    if (++terminal_column == VGA_WIDTH) {
        terminal_newline();
    }
    
    terminal_update_cursor();
}

// Write string to terminal
void terminal_writestring(const char* str) {
    while (*str) {
        terminal_putchar(*str++);
    }
}

// Write string with color
void terminal_writestring_color(const char* str, uint8_t color) {
    uint8_t old_color = terminal_color;
    terminal_color = color;
    terminal_writestring(str);
    terminal_color = old_color;
}

// Set terminal color
void terminal_setcolor(uint8_t fg, uint8_t bg) {
    terminal_color = vga_entry_color(fg, bg);
}

// Get current terminal color
uint8_t terminal_getcolor(void) {
    return terminal_color;
}

// Set cursor position
void terminal_setcursor(size_t x, size_t y) {
    if (x >= VGA_WIDTH) x = VGA_WIDTH - 1;
    if (y >= VGA_HEIGHT) y = VGA_HEIGHT - 1;
    
    terminal_column = x;
    terminal_row = y;
    terminal_update_cursor();
}

// Get cursor position
void terminal_getcursor(size_t* x, size_t* y) {
    if (x) *x = terminal_column;
    if (y) *y = terminal_row;
}

// Update hardware cursor
void terminal_update_cursor(void) {
    uint16_t pos = terminal_row * VGA_WIDTH + terminal_column;
    
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

// Enable/disable cursor
void terminal_enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
    
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void terminal_disable_cursor(void) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

// Handle newline
void terminal_newline(void) {
    terminal_column = 0;
    
    if (++terminal_row == VGA_HEIGHT) {
        terminal_scroll();
    }
}

// Handle backspace
void terminal_backspace(void) {
    if (terminal_column > 0) {
        terminal_column--;
    } else if (terminal_row > 0) {
        terminal_row--;
        terminal_column = VGA_WIDTH - 1;
    }
    
    // Clear character at position
    terminal_buffer[terminal_row * VGA_WIDTH + terminal_column] = 
        vga_entry(' ', terminal_color);
    
    terminal_update_cursor();
}

// Scroll screen up by one line
void terminal_scroll(void) {
    // Move all lines up by one
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = 
                terminal_buffer[(y + 1) * VGA_WIDTH + x];
        }
    }
    
    // Clear bottom line
    uint16_t blank = vga_entry(' ', terminal_color);
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
    }
    
    terminal_row = VGA_HEIGHT - 1;
}

// Put character at specific position with color
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
        return;
    }
    
    terminal_buffer[y * VGA_WIDTH + x] = vga_entry(c, color);
}

// Clear line from current position to end
void terminal_cleareol(void) {
    uint16_t blank = vga_entry(' ', terminal_color);
    
    for (size_t x = terminal_column; x < VGA_WIDTH; x++) {
        terminal_buffer[terminal_row * VGA_WIDTH + x] = blank;
    }
}

// Clear entire line
void terminal_clearln(size_t y) {
    if (y >= VGA_HEIGHT) return;
    
    uint16_t blank = vga_entry(' ', terminal_color);
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[y * VGA_WIDTH + x] = blank;
    }
}

// Print with color
void print_color(const char* str, uint8_t fg, uint8_t bg) {
    uint8_t old_color = terminal_color;
    terminal_setcolor(fg, bg);
    terminal_writestring(str);
    terminal_color = old_color;
}

// Print formatted string (basic)
void printf(const char* format, ...) {
    // Simple implementation - just print string for now
    // Full printf implementation would be complex
    terminal_writestring(format);
}
