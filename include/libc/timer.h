#ifndef TIMER_H
#define TIMER_H

#include "types.h"

void timer_init(uint32_t frequency_hz);
void sleep_ms(uint32_t milliseconds);
uint64_t get_ticks(void);
void timer_interrupt_handler(void);

#define TIMER_FREQ 1000
#define TICKS_PER_MS 1

#endif
