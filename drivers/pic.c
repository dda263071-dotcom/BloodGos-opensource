/**************************************************************
 * PIC (8259) Driver - BloodG OS
 * Handles hardware interrupts
 **************************************************************/

#include <stdint.h>
#include "io.h"

// PIC ports
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

// PIC commands
#define PIC_EOI         0x20  // End of interrupt

// ICW1 (Initialization Command Word 1)
#define PIC_ICW1_ICW4       0x01  // ICW4 needed
#define PIC_ICW1_SINGLE     0x02  // Single mode
#define PIC_ICW1_INTERVAL4  0x04  // Call address interval 4
#define PIC_ICW1_LEVEL      0x08  // Level triggered mode
#define PIC_ICW1_INIT       0x10  // Initialization

// ICW4 (Initialization Command Word 4)
#define PIC_ICW4_8086       0x01  // 8086/88 mode
#define PIC_ICW4_AUTO_EOI   0x02  // Auto EOI
#define PIC_ICW4_BUF_SLAVE  0x08  // Buffered mode/slave
#define PIC_ICW4_BUF_MASTER 0x0C  // Buffered mode/master
#define PIC_ICW4_SFNM       0x10  // Special fully nested mode

// Remap PIC interrupts to avoid conflicts with CPU exceptions
#define PIC1_OFFSET         0x20  // IRQ 0-7 -> INT 0x20-0x27
#define PIC2_OFFSET         0x28  // IRQ 8-15 -> INT 0x28-0x2F

// Initialize PIC with custom offsets
void pic_init(void) {
    print_string("Initializing PIC...\n");
    
    // Save masks
    uint8_t pic1_mask = inb(PIC1_DATA);
    uint8_t pic2_mask = inb(PIC2_DATA);
    
    // Start initialization sequence (ICW1)
    outb(PIC1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();
    
    // Set vector offsets (ICW2)
    outb(PIC1_DATA, PIC1_OFFSET);
    io_wait();
    outb(PIC2_DATA, PIC2_OFFSET);
    io_wait();
    
    // Tell PIC1 about PIC2 at IRQ2 (ICW3)
    outb(PIC1_DATA, 0x04);  // PIC2 at IRQ2
    io_wait();
    
    // Tell PIC2 its cascade identity (ICW3)
    outb(PIC2_DATA, 0x02);  // Cascade identity
    io_wait();
    
    // Set 8086 mode (ICW4)
    outb(PIC1_DATA, PIC_ICW4_8086);
    io_wait();
    outb(PIC2_DATA, PIC_ICW4_8086);
    io_wait();
    
    // Restore masks
    outb(PIC1_DATA, pic1_mask);
    outb(PIC2_DATA, pic2_mask);
    
    print_string("PIC: Remapped IRQs 0-7 to 0x20-0x27, IRQs 8-15 to 0x28-0x2F\n");
}

// Send End of Interrupt to PIC
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

// Disable PIC (mask all interrupts)
void pic_disable(void) {
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

// Enable specific IRQ
void pic_enable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

// Disable specific IRQ
void pic_disable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    
    value = inb(port) | (1 << irq);
    outb(port, value);
}

// Get IRR (Interrupt Request Register)
uint16_t pic_get_irr(void) {
    outb(PIC1_COMMAND, 0x0A);  // Command: read IRR
    outb(PIC2_COMMAND, 0x0A);
    
    uint8_t irr1 = inb(PIC1_COMMAND);
    uint8_t irr2 = inb(PIC2_COMMAND);
    
    return (irr2 << 8) | irr1;
}

// Get ISR (In-Service Register)
uint16_t pic_get_isr(void) {
    outb(PIC1_COMMAND, 0x0B);  // Command: read ISR
    outb(PIC2_COMMAND, 0x0B);
    
    uint8_t isr1 = inb(PIC1_COMMAND);
    uint8_t isr2 = inb(PIC2_COMMAND);
    
    return (isr2 << 8) | isr1;
}

// Mask all interrupts except cascade
void pic_mask_all(void) {
    outb(PIC1_DATA, 0xFF & ~(1 << 2));  // Keep IRQ2 (cascade) enabled
    outb(PIC2_DATA, 0xFF);
}

// Unmask all interrupts
void pic_unmask_all(void) {
    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);
}

// Get interrupt mask
uint16_t pic_get_mask(void) {
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);
    
    return (mask2 << 8) | mask1;
}

// Set interrupt mask
void pic_set_mask(uint16_t mask) {
    outb(PIC1_DATA, mask & 0xFF);
    outb(PIC2_DATA, (mask >> 8) & 0xFF);
}
