/**************************************************************
 * Timer Driver Header - BloodG OS
 * PIT (Programmable Interval Timer) interface
 **************************************************************/

#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>

/* ==================== TIMER CONSTANTS ==================== */

#define TIMER_BASE_FREQ    1193182  // PIT base frequency (Hz)
#define TIMER_DEFAULT_HZ   1000     // 1ms ticks
#define TIMER_MIN_HZ       19       // Minimum frequency
#define TIMER_MAX_HZ       TIMER_BASE_FREQ  // Maximum frequency

/* ==================== TIMER FUNCTIONS ==================== */

/**
 * Initialize timer with specified frequency
 * @param frequency Frequency in Hz (19-1193182)
 */
void timer_init(uint32_t frequency);

/**
 * Timer interrupt handler (called from ISR)
 */
void timer_handler(void);

/**
 * Get current tick count
 * @return Number of ticks since boot
 */
uint32_t timer_get_ticks(void);

/**
 * Get timer frequency
 * @return Timer frequency in Hz
 */
uint32_t timer_get_frequency(void);

/**
 * Convert ticks to milliseconds
 * @param ticks Number of ticks
 * @return Milliseconds
 */
uint32_t timer_ticks_to_ms(uint32_t ticks);

/**
 * Convert ticks to microseconds
 * @param ticks Number of ticks
 * @return Microseconds
 */
uint32_t timer_ticks_to_us(uint32_t ticks);

/**
 * Sleep for specified milliseconds (busy wait)
 * @param milliseconds Milliseconds to sleep
 */
void timer_sleep_ms(uint32_t milliseconds);

/**
 * Sleep for specified microseconds (busy wait)
 * @param microseconds Microseconds to sleep
 */
void timer_sleep_us(uint32_t microseconds);

/**
 * Get current time in milliseconds
 * @return Milliseconds since boot
 */
uint32_t timer_get_ms(void);

/**
 * Get current time in microseconds
 * @return Microseconds since boot
 */
uint64_t timer_get_us(void);

/**
 * Simple delay using port I/O
 * @param count Number of iterations
 */
void delay_io(uint32_t count);

/**
 * Calibrate timer (returns configured frequency)
 * @return Timer frequency in Hz
 */
uint32_t timer_calibrate(void);

/* ==================== UTILITY MACROS ==================== */

/**
 * Calculate ticks for given milliseconds
 */
#define MS_TO_TICKS(ms) ((ms) * timer_get_frequency() / 1000)

/**
 * Calculate ticks for given microseconds
 */
#define US_TO_TICKS(us) ((us) * timer_get_frequency() / 1000000)

#endif /* _TIMER_H */
