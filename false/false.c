/**************************************************************
 * Undefined Function Handler - BloodG OS
 * Catches missing function implementations
 **************************************************************/

#include <stdint.h>
#include <stddef.h>

// VGA access
#define VGA_MEMORY ((uint16_t*)0xB8000)
#define VGA_WIDTH 80

static void write_message(const char* str) {
    uint8_t color = 0x4E; // Yellow on red
    int y = 12;
    int x = 20;
    
    for (int i = 0; str[i] && x + i < VGA_WIDTH; i++) {
        VGA_MEMORY[y * VGA_WIDTH + x + i] = (uint16_t)str[i] | ((uint16_t)color << 8);
    }
}

// Undefined function handler
void __undefined_function(const char* func_name) {
    // Simple error display
    write_message("UNDEFINED FUNCTION CALLED");
    
    // Try to display function name
    if (func_name) {
        // Move to next line
        int y = 13;
        int x = 20;
        for (int i = 0; func_name[i] && x + i < VGA_WIDTH; i++) {
            VGA_MEMORY[y * VGA_WIDTH + x + i] = (uint16_t)func_name[i] | ((uint16_t)0x4E << 8);
        }
    }
    
    // Halt
    asm volatile ("cli");
    while (1) {
        asm volatile ("hlt");
    }
}

// Common undefined functions from libc
void __stack_chk_fail(void) {
    __undefined_function("__stack_chk_fail");
}

void abort(void) {
    __undefined_function("abort");
}

void exit(int status) {
    (void)status;
    __undefined_function("exit");
}

// Pure virtual function call (for C++ if needed)
void __cxa_pure_virtual(void) {
    __undefined_function("__cxa_pure_virtual");
}

// Memory allocation errors
void* __attribute__((weak)) malloc(size_t size) {
    (void)size;
    __undefined_function("malloc");
    return NULL;
}

void __attribute__((weak)) free(void* ptr) {
    (void)ptr;
    __undefined_function("free");
}

// Standard I/O functions
int __attribute__((weak)) printf(const char* format, ...) {
    (void)format;
    __undefined_function("printf");
    return 0;
}

int __attribute__((weak)) puts(const char* str) {
    (void)str;
    __undefined_function("puts");
    return 0;
}

// String functions that might be missing
char* __attribute__((weak)) strdup(const char* s) {
    (void)s;
    __undefined_function("strdup");
    return NULL;
}

// Math functions
double __attribute__((weak)) sin(double x) {
    (void)x;
    __undefined_function("sin");
    return 0.0;
}

double __attribute__((weak)) cos(double x) {
    (void)x;
    __undefined_function("cos");
    return 0.0;
}

// Set up undefined function handler
void setup_undefined_handlers(void) {
    // This function can be called early to set up handlers
    // Currently handlers are defined weakly, so they'll be used automatically
}
