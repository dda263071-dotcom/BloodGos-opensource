/**************************************************************
 * Kernel Panic Handler - BloodG OS
 * Emergency error handler for kernel failures
 **************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Direct VGA access (no dependencies)
#define VGA_MEMORY ((uint16_t*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Colors
#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GREY 7
#define COLOR_DARK_GREY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_YELLOW 14
#define COLOR_WHITE 15

// Create color byte
static inline uint8_t make_color(uint8_t fg, uint8_t bg) {
    return fg | (bg << 4);
}

// Create VGA entry
static inline uint16_t make_vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

// Write character at position
static void write_char_at(char c, uint8_t color, size_t x, size_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) return;
    VGA_MEMORY[y * VGA_WIDTH + x] = make_vga_entry(c, color);
}

// Write string at position
static void write_string_at(const char* str, uint8_t color, size_t x, size_t y) {
    while (*str) {
        write_char_at(*str, color, x, y);
        str++;
        x++;
        if (x >= VGA_WIDTH) break;
    }
}

// Clear screen with color
static void clear_screen(uint8_t color) {
    uint16_t blank = make_vga_entry(' ', color);
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEMORY[i] = blank;
    }
}

// Draw box
static void draw_box(size_t x1, size_t y1, size_t x2, size_t y2, uint8_t color) {
    // Draw corners
    write_char_at('╔', color, x1, y1);
    write_char_at('╗', color, x2, y1);
    write_char_at('╚', color, x1, y2);
    write_char_at('╝', color, x2, y2);
    
    // Draw horizontal lines
    for (size_t x = x1 + 1; x < x2; x++) {
        write_char_at('═', color, x, y1);
        write_char_at('═', color, x, y2);
    }
    
    // Draw vertical lines
    for (size_t y = y1 + 1; y < y2; y++) {
        write_char_at('║', color, x1, y);
        write_char_at('║', color, x2, y);
    }
}

// Kernel panic - unrecoverable error
void kernel_panic(const char* message, const char* file, int line) {
    // Disable interrupts
    asm volatile ("cli");
    
    // Clear screen with red background
    clear_screen(make_color(COLOR_WHITE, COLOR_RED));
    
    // Draw panic box
    draw_box(10, 5, 69, 19, make_color(COLOR_WHITE, COLOR_RED));
    
    // Display panic header
    write_string_at("╔══════════════════════════════════════════════════════╗", 
                   make_color(COLOR_WHITE, COLOR_RED), 10, 5);
    write_string_at("║               KERNEL PANIC                           ║", 
                   make_color(COLOR_WHITE, COLOR_RED), 10, 6);
    write_string_at("╚══════════════════════════════════════════════════════╝", 
                   make_color(COLOR_WHITE, COLOR_RED), 10, 7);
    
    // Display error message
    write_string_at("Error:", make_color(COLOR_YELLOW, COLOR_RED), 12, 9);
    write_string_at(message, make_color(COLOR_WHITE, COLOR_RED), 20, 9);
    
    // Display location
    write_string_at("Location:", make_color(COLOR_YELLOW, COLOR_RED), 12, 11);
    write_string_at(file, make_color(COLOR_WHITE, COLOR_RED), 22, 11);
    
    // Display line number
    char line_str[16];
    char* p = line_str;
    int n = line;
    
    // Convert line number to string
    if (n == 0) {
        *p++ = '0';
    } else {
        char temp[16];
        char* t = temp;
        while (n > 0) {
            *t++ = '0' + (n % 10);
            n /= 10;
        }
        while (t > temp) {
            *p++ = *(--t);
        }
    }
    *p = '\0';
    
    write_string_at("Line:", make_color(COLOR_YELLOW, COLOR_RED), 12, 12);
    write_string_at(line_str, make_color(COLOR_WHITE, COLOR_RED), 18, 12);
    
    // Display register dump
    write_string_at("Register Dump:", make_color(COLOR_YELLOW, COLOR_RED), 12, 14);
    
    uint32_t eax, ebx, ecx, edx, esp, ebp;
    asm volatile (
        "mov %%eax, %0\n"
        "mov %%ebx, %1\n"
        "mov %%ecx, %2\n"
        "mov %%edx, %3\n"
        "mov %%esp, %4\n"
        "mov %%ebp, %5\n"
        : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx), "=r"(esp), "=r"(ebp)
    );
    
    // Display registers (simplified)
    write_string_at("EAX=", make_color(COLOR_CYAN, COLOR_RED), 12, 15);
    write_string_at("EBX=", make_color(COLOR_CYAN, COLOR_RED), 32, 15);
    write_string_at("ECX=", make_color(COLOR_CYAN, COLOR_RED), 52, 15);
    
    write_string_at("EDX=", make_color(COLOR_CYAN, COLOR_RED), 12, 16);
    write_string_at("ESP=", make_color(COLOR_CYAN, COLOR_RED), 32, 16);
    write_string_at("EBP=", make_color(COLOR_CYAN, COLOR_RED), 52, 16);
    
    // Display instructions
    write_string_at("System halted. Press reset button.", 
                   make_color(COLOR_YELLOW, COLOR_RED), 12, 18);
    
    // Halt forever
    while (1) {
        asm volatile ("hlt");
    }
}

// Assertion failure
void assert_failed(const char* expr, const char* file, int line) {
    kernel_panic(expr, file, line);
}
