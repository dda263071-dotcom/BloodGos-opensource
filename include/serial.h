/**************************************************************
 * Serial Port Driver Header - BloodG OS
 * COM1 serial communication interface
 **************************************************************/

#ifndef _SERIAL_H
#define _SERIAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ==================== SERIAL CONSTANTS ==================== */

#define SERIAL_COM1    0x3F8
#define SERIAL_COM2    0x2F8
#define SERIAL_COM3    0x3E8
#define SERIAL_COM4    0x2E8

#define SERIAL_BAUD_115200  115200
#define SERIAL_BAUD_57600   57600
#define SERIAL_BAUD_38400   38400
#define SERIAL_BAUD_19200   19200
#define SERIAL_BAUD_9600    9600

/* ==================== REGISTER OFFSETS ==================== */

#define SERIAL_DATA_REG(base)          (base)
#define SERIAL_INT_ENABLE_REG(base)    (base + 1)
#define SERIAL_FIFO_CTRL_REG(base)     (base + 2)
#define SERIAL_LINE_CTRL_REG(base)     (base + 3)
#define SERIAL_MODEM_CTRL_REG(base)    (base + 4)
#define SERIAL_LINE_STATUS_REG(base)   (base + 5)
#define SERIAL_MODEM_STATUS_REG(base)  (base + 6)
#define SERIAL_SCRATCH_REG(base)       (base + 7)

/* ==================== LINE STATUS BITS ==================== */

#define SERIAL_LSR_DATA_READY      0x01
#define SERIAL_LSR_OVERRUN_ERROR   0x02
#define SERIAL_LSR_PARITY_ERROR    0x04
#define SERIAL_LSR_FRAMING_ERROR   0x08
#define SERIAL_LSR_BREAK_INDICATOR 0x10
#define SERIAL_LSR_TX_HOLD_EMPTY   0x20
#define SERIAL_LSR_TX_EMPTY        0x40
#define SERIAL_LSR_ERROR           0x80

/* ==================== SERIAL FUNCTIONS ==================== */

/**
 * Initialize serial port
 * @param port Port address (COM1-COM4)
 * @return true if successful, false otherwise
 */
bool serial_init(uint16_t port);

/**
 * Initialize default serial port (COM1)
 * @return true if successful, false otherwise
 */
bool serial_init_default(void);

/**
 * Write character to serial port
 * @param port Port address
 * @param c Character to write
 */
void serial_putc(uint16_t port, char c);

/**
 * Write character to default port (COM1)
 * @param c Character to write
 */
void serial_putc_default(char c);

/**
 * Write string to serial port
 * @param port Port address
 * @param str String to write
 */
void serial_puts(uint16_t port, const char* str);

/**
 * Write string to default port (COM1)
 * @param str String to write
 */
void serial_puts_default(const char* str);

/**
 * Read character from serial port
 * @param port Port address
 * @return Character read
 */
char serial_getc(uint16_t port);

/**
 * Read character from default port (COM1)
 * @return Character read
 */
char serial_getc_default(void);

/**
 * Check if data is available
 * @param port Port address
 * @return true if data available, false otherwise
 */
bool serial_data_available(uint16_t port);

/**
 * Check if transmitter is empty
 * @param port Port address
 * @return true if ready to transmit, false otherwise
 */
bool serial_transmit_empty(uint16_t port);

/**
 * Read line from serial port
 * @param port Port address
 * @param buffer Buffer to store string
 * @param max_len Maximum buffer length
 */
void serial_readline(uint16_t port, char* buffer, size_t max_len);

/**
 * Simple formatted output to serial
 * @param port Port address
 * @param format Format string
 * @param ... Arguments
 */
void serial_printf(uint16_t port, const char* format, ...);

/**
 * Get line status register
 * @param port Port address
 * @return Line status byte
 */
uint8_t serial_get_line_status(uint16_t port);

/**
 * Get modem status register
 * @param port Port address
 * @return Modem status byte
 */
uint8_t serial_get_modem_status(uint16_t port);

/**
 * Set baud rate
 * @param port Port address
 * @param baud_rate Baud rate (9600, 19200, 38400, 57600, 115200)
 * @return true if successful, false otherwise
 */
bool serial_set_baud(uint16_t port, uint32_t baud_rate);

/**
 * Set line parameters
 * @param port Port address
 * @param data_bits Data bits (5-8)
 * @param stop_bits Stop bits (1-2)
 * @param parity Parity (0=none, 1=odd, 2=even)
 * @return true if successful, false otherwise
 */
bool serial_set_line(uint16_t port, uint8_t data_bits, uint8_t stop_bits, uint8_t parity);

/* ==================== UTILITY FUNCTIONS ==================== */

/**
 * Wait for data to be available
 * @param port Port address
 */
static inline void serial_wait_for_data(uint16_t port) {
    while (!serial_data_available(port));
}

/**
 * Wait for transmitter to be ready
 * @param port Port address
 */
static inline void serial_wait_for_transmit(uint16_t port) {
    while (!serial_transmit_empty(port));
}

#endif /* _SERIAL_H */
