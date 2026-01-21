/**************************************************************
 * PIT Timer Driver - BloodG OS
 * Provides timing and scheduling functions
 **************************************************************/

#include <stdint.h>
#include "io.h"

// PIT ports
#define PIT_CHANNEL0    0x40
#define PIT_CHANNEL1    0x41
#define PIT_CHANNEL2    0x42
#define PIT_COMMAND     0x43

// PIT commands
#define PIT_CMD_CHANNEL0    0x00
#define PIT_CMD_CHANNEL1    0x40
#define PIT_CMD_CHANNEL2    0x80
#define PIT_CMD_READBACK    0xC0

#define PIT_CMD_LATCH       0x00
#define PIT_CMD_LOBYTE      0x10
#define PIT_CMD_HIBYTE      0x20
#define PIT_CMD_LOHIBYTE    0x30

#define PIT_CMD_MODE0       0x00  // Interrupt on terminal count
#define PIT_CMD_MODE1       0x02  // Hardware retriggerable one-shot
#define PIT_CMD_MODE2       0x04  // Rate generator
#define PIT_CMD_MODE3       0x06  // Square wave generator
#define PIT_CMD_MODE4       0x08  // Software triggered strobe
#define PIT_CMD_MODE5       0x0A  // Hardware triggered strobe

#define PIT_CMD_BINARY      0x00
#define PIT_CMD_BCD         0x01

// PIT frequency (Hz)
#define PIT_BASE_FREQ       1193182
#define PIT_DEFAULT_HZ      1000  // 1ms ticks

// Timer state
static volatile uint32_t timer_ticks = 0;
static uint32_t timer_frequency = PIT_DEFAULT_HZ;

// Initialize PIT
void timer_init(uint32_t frequency) {
    if (frequency < 19) frequency = 19;    // Minimum frequency
    if (frequency > PIT_BASE_FREQ) frequency = PIT_BASE_FREQ;
    
    timer_frequency = frequency;
    
    // Calculate divisor
    uint16_t divisor = PIT_BASE_FREQ / frequency;
    
    // Send command byte
    outb(PIT_COMMAND, 
         PIT_CMD_CHANNEL0 | 
         PIT_CMD_LOHIBYTE | 
         PIT_CMD_MODE3 | 
         PIT_CMD_BINARY);
    
    // Send divisor (low then high byte)
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
    
    // Reset tick counter
    timer_ticks = 0;
    
    // Print initialization message
    char freq_str[16];
    utoa(frequency, freq_str, 10);
    print_string("Timer: Initialized at ");
    print_string(freq_str);
    print_string(" Hz\n");
}

// Timer interrupt handler (called from ISR)
void timer_handler(void) {
    timer_ticks++;
}

// Get current tick count
uint32_t timer_get_ticks(void) {
    return timer_ticks;
}

// Get timer frequency
uint32_t timer_get_frequency(void) {
    return timer_frequency;
}

// Calculate milliseconds from ticks
uint32_t timer_ticks_to_ms(uint32_t ticks) {
    return (ticks * 1000) / timer_frequency;
}

// Calculate microseconds from ticks
uint32_t timer_ticks_to_us(uint32_t ticks) {
    return (ticks * 1000000) / timer_frequency;
}

// Sleep for specified milliseconds (busy wait)
void timer_sleep_ms(uint32_t milliseconds) {
    uint32_t start_ticks = timer_ticks;
    uint32_t target_ticks = start_ticks + 
                           (milliseconds * timer_frequency / 1000);
    
    while (timer_ticks < target_ticks) {
        // Busy wait - could be improved with interrupts
        asm volatile ("pause");
    }
}

// Sleep for specified microseconds (busy wait)
void timer_sleep_us(uint32_t microseconds) {
    uint32_t start_ticks = timer_ticks;
    uint32_t target_ticks = start_ticks + 
                           (microseconds * timer_frequency / 1000000);
    
    while (timer_ticks < target_ticks) {
        asm volatile ("pause");
    }
}

// Get current time in milliseconds
uint32_t timer_get_ms(void) {
    return timer_ticks_to_ms(timer_ticks);
}

// Get current time in microseconds
uint64_t timer_get_us(void) {
    uint64_t ticks = timer_ticks;
    return (ticks * 1000000) / timer_frequency;
}

// Simple delay using port I/O (alternative to timer)
void delay_io(uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        io_wait();
    }
}

// Calibrate timer (measure actual frequency)
uint32_t timer_calibrate(void) {
    // This would require a reference clock
    // For now, just return configured frequency
    return timer_frequency;
}
