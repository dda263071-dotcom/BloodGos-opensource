#include "timer.h"
#include "io.h"

static volatile uint64_t tick_count = 0;

void timer_init(uint32_t frequency_hz) {
    uint32_t divisor = 1193180 / frequency_hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
    tick_count = 0;
}

void sleep_ms(uint32_t milliseconds) {
    uint64_t target = tick_count + milliseconds;
    while (tick_count < target) {
        __asm__ volatile("pause");
    }
}

uint64_t get_ticks(void) {
    return tick_count;
}

void timer_interrupt_handler(void) {
    tick_count++;
    outb(0x20, 0x20);   // EOI untuk PIC master
}
