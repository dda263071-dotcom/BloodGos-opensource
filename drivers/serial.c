/**************************************************************
 * Serial Port Driver - BloodG OS
 * Provides serial communication (COM1)
 **************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "io.h"
#include "string.h"

// Serial port addresses
#define COM1_PORT   0x3F8
#define COM2_PORT   0x2F8
#define COM3_PORT   0x3E8
#define COM4_PORT   0x2E8

// Serial port registers
#define SERIAL_DATA(base)          (base)
#define SERIAL_INTERRUPT_ENABLE(base) (base + 1)
#define SERIAL_FIFO_CONTROL(base)  (base + 2)
#define SERIAL_LINE_CONTROL(base)  (base + 3)
#define SERIAL_MODEM_CONTROL(base) (base + 4)
#define SERIAL_LINE_STATUS(base)   (base + 5)
#define SERIAL_MODEM_STATUS(base)  (base + 6)
#define SERIAL_SCRATCH(base)       (base + 7)

// Line status bits
#define SERIAL_LSR_DATA_READY      0x01
#define SERIAL_LSR_OVERRUN_ERROR   0x02
#define SERIAL_LSR_PARITY_ERROR    0x04
#define SERIAL_LSR_FRAMING_ERROR   0x08
#define SERIAL_LSR_BREAK_INDICATOR 0x10
#define SERIAL_LSR_TRANSMIT_HOLD_EMPTY 0x20
#define SERIAL_LSR_TRANSMIT_EMPTY  0x40
#define SERIAL_LSR_IMPENDING_ERROR 0x80

// Line control bits
#define SERIAL_LCR_DLAB           0x80  // Divisor Latch Access Bit

// Initialize serial port
bool serial_init(uint16_t port) {
    // Disable interrupts
    outb(SERIAL_INTERRUPT_ENABLE(port), 0x00);
    
    // Enable DLAB (set baud rate divisor)
    outb(SERIAL_LINE_CONTROL(port), SERIAL_LCR_DLAB);
    
    // Set divisor to 3 (38400 baud)
    // 115200 / 3 = 38400
    outb(SERIAL_DATA(port), 0x03);       // Low byte
    outb(SERIAL_DATA(port) + 1, 0x00);   // High byte
    
    // 8 bits, no parity, one stop bit
    outb(SERIAL_LINE_CONTROL(port), 0x03);
    
    // Enable FIFO, clear them, with 14-byte threshold
    outb(SERIAL_FIFO_CONTROL(port), 0xC7);
    
    // IRQs enabled, RTS/DSR set
    outb(SERIAL_MODEM_CONTROL(port), 0x0B);
    
    // Test serial chip
    outb(SERIAL_MODEM_CONTROL(port), 0x1E);  // Set loopback mode
    outb(SERIAL_DATA(port), 0xAE);           // Send test byte
    
    // Check if same byte received
    if (inb(SERIAL_DATA(port)) != 0xAE) {
        return false;  // Serial port not working
    }
    
    // Set normal operation
    outb(SERIAL_MODEM_CONTROL(port), 0x0F);
    
    return true;
}

// Initialize default serial port (COM1)
bool serial_init_default(void) {
    print_string("Initializing serial port (COM1)...\n");
    
    if (!serial_init(COM1_PORT)) {
        print_string("Serial: COM1 initialization failed\n");
        return false;
    }
    
    print_string("Serial: COM1 initialized at 38400 baud\n");
    return true;
}

// Check if transmit is empty
static bool serial_transmit_empty(uint16_t port) {
    return (inb(SERIAL_LINE_STATUS(port)) & SERIAL_LSR_TRANSMIT_HOLD_EMPTY) != 0;
}

// Write character to serial port
void serial_putc(uint16_t port, char c) {
    while (!serial_transmit_empty(port)) {
        // Wait for transmit to be ready
    }
    
    outb(port, c);
}

// Write character to default port (COM1)
void serial_putc_default(char c) {
    serial_putc(COM1_PORT, c);
}

// Write string to serial port
void serial_puts(uint16_t port, const char* str) {
    while (*str) {
        serial_putc(port, *str++);
    }
}

// Write string to default port
void serial_puts_default(const char* str) {
    serial_puts(COM1_PORT, str);
}

// Check if data is available
static bool serial_received(uint16_t port) {
    return (inb(SERIAL_LINE_STATUS(port)) & SERIAL_LSR_DATA_READY) != 0;
}

// Read character from serial port
char serial_getc(uint16_t port) {
    while (!serial_received(port)) {
        // Wait for data
    }
    
    return inb(port);
}

// Read character from default port
char serial_getc_default(void) {
    return serial_getc(COM1_PORT);
}

// Read line from serial port (with echo)
void serial_readline(uint16_t port, char* buffer, size_t max_len) {
    size_t pos = 0;
    
    while (pos < max_len - 1) {
        char c = serial_getc(port);
        
        // Echo back (optional)
        serial_putc(port, c);
        
        if (c == '\r' || c == '\n') {
            buffer[pos] = '\0';
            serial_putc(port, '\n');  // Newline after enter
            return;
        } else if (c == '\b' || c == 0x7F) {  // Backspace or delete
            if (pos > 0) {
                pos--;
                serial_puts(port, "\b \b");  // Erase character
            }
        } else if (c >= 32 && c <= 126) {  // Printable ASCII
            buffer[pos++] = c;
        }
    }
    
    buffer[pos] = '\0';
}

// Print formatted output to serial (basic)
void serial_printf(uint16_t port, const char* format, ...) {
    // Simple implementation - just send string
    serial_puts(port, format);
}

// Get serial port status
uint8_t serial_get_status(uint16_t port) {
    return inb(SERIAL_LINE_STATUS(port));
}

// Get modem status
uint8_t serial_get_modem_status(uint16_t port) {
    return inb(SERIAL_MODEM_STATUS(port));
}

// Set baud rate
bool serial_set_baud(uint16_t port, uint32_t baud_rate) {
    if (baud_rate == 0) return false;
    
    uint16_t divisor = 115200 / baud_rate;
    if (divisor == 0) divisor = 1;
    
    // Enable DLAB
    outb(SERIAL_LINE_CONTROL(port), SERIAL_LCR_DLAB);
    
    // Set divisor
    outb(SERIAL_DATA(port), divisor & 0xFF);
    outb(SERIAL_DATA(port) + 1, (divisor >> 8) & 0xFF);
    
    // Disable DLAB
    outb(SERIAL_LINE_CONTROL(port), 0x03);
    
    return true;
}
