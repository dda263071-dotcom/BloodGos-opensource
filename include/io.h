/**************************************************************
 * I/O Port Operations Header - BloodG OS
 **************************************************************/

#ifndef _IO_H
#define _IO_H

#include <stdint.h>

/* ==================== PORT I/O FUNCTIONS ==================== */

/**
 * Read byte from port
 * @param port Port address
 * @return Byte read from port
 */
uint8_t inb(uint16_t port);

/**
 * Write byte to port
 * @param port Port address
 * @param value Byte to write
 */
void outb(uint16_t port, uint8_t value);

/**
 * Read word from port
 * @param port Port address
 * @return Word read from port
 */
uint16_t inw(uint16_t port);

/**
 * Write word to port
 * @param port Port address
 * @param value Word to write
 */
void outw(uint16_t port, uint16_t value);

/**
 * Read double word from port
 * @param port Port address
 * @return Double word read from port
 */
uint32_t inl(uint16_t port);

/**
 * Write double word to port
 * @param port Port address
 * @param value Double word to write
 */
void outl(uint16_t port, uint32_t value);

/* ==================== SERIAL PORT FUNCTIONS ==================== */

/**
 * Initialize serial port (COM1)
 */
void serial_init(void);

/**
 * Write character to serial port
 * @param c Character to write
 */
void serial_putc(char c);

/**
 * Write string to serial port
 * @param str String to write
 */
void serial_puts(const char* str);

/**
 * Read character from serial port
 * @return Character read
 */
char serial_getc(void);

/**
 * Check if serial data is available
 * @return Non-zero if data available, 0 otherwise
 */
int serial_received(void);

/**
 * Check if serial transmitter is empty
 * @return Non-zero if ready to transmit, 0 otherwise
 */
int serial_transmit_empty(void);

/* ==================== CONTROL REGISTERS ==================== */

/**
 * Read CR0 register
 * @return Value of CR0
 */
uint32_t read_cr0(void);

/**
 * Write CR0 register
 * @param val Value to write
 */
void write_cr0(uint32_t val);

/**
 * Read CR2 register (page fault linear address)
 * @return Value of CR2
 */
uint32_t read_cr2(void);

/**
 * Read CR3 register (page directory base)
 * @return Value of CR3
 */
uint32_t read_cr3(void);

/**
 * Write CR3 register
 * @param val Value to write
 */
void write_cr3(uint32_t val);

/**
 * Read CR4 register
 * @return Value of CR4
 */
uint32_t read_cr4(void);

/**
 * Write CR4 register
 * @param val Value to write
 */
void write_cr4(uint32_t val);

/* ==================== INTERRUPT CONTROL ==================== */

/**
 * Disable interrupts
 */
void cli(void);

/**
 * Enable interrupts
 */
void sti(void);

/**
 * Halt CPU (wait for interrupt)
 */
void halt(void);

/* ==================== CPU CONTROL ==================== */

/**
 * CPU wait (short delay for I/O)
 */
void io_wait(void);

/**
 * Memory barrier (ensure memory operations complete)
 */
void memory_barrier(void);

/**
 * Read EFLAGS register
 * @return Value of EFLAGS
 */
uint32_t read_eflags(void);

#endif // _IO_H
