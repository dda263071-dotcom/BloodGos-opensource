/**************************************************************
 * I/O Utilities
 **************************************************************/

#include <stdint.h>

// Serial port functions
void serial_init(void) {
    outb(0x3F8 + 1, 0x00);    // Disable interrupts
    outb(0x3F8 + 3, 0x80);    // Enable DLAB
    outb(0x3F8 + 0, 0x03);    // Set baud rate divisor low byte
    outb(0x3F8 + 1, 0x00);    // Set baud rate divisor high byte
    outb(0x3F8 + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(0x3F8 + 2, 0xC7);    // Enable FIFO
    outb(0x3F8 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void serial_putc(char c) {
    while ((inb(0x3F8 + 5) & 0x20) == 0);
    outb(0x3F8, c);
}

void serial_puts(const char* str) {
    while (*str) {
        serial_putc(*str++);
    }
}

// Memory I/O
uint32_t read_cr0(void) {
    uint32_t val;
    asm volatile ("mov %%cr0, %0" : "=r"(val));
    return val;
}

void write_cr0(uint32_t val) {
    asm volatile ("mov %0, %%cr0" : : "r"(val));
}

uint32_t read_cr2(void) {
    uint32_t val;
    asm volatile ("mov %%cr2, %0" : "=r"(val));
    return val;
}

uint32_t read_cr3(void) {
    uint32_t val;
    asm volatile ("mov %%cr3, %0" : "=r"(val));
    return val;
}

// Interrupt control
void cli(void) {
    asm volatile ("cli");
}

void sti(void) {
    asm volatile ("sti");
}
