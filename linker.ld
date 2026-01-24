/*
 * BloodGos OS Kernel Linker Script
 * Menghubungkan semua komponen: Bootloader → Kernel → Drivers → Filesystem
 * Target: x86 32-bit Protected Mode
 */

/* Entry point - harus sama dengan label di kernel_entry.asm */
ENTRY(_start)

/* Output format ELF untuk x86 32-bit */
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)

/* 
 * Memory Layout:
 * 0x00000000-0x000FFFFF: Reserved (BIOS, Bootloader, Low Memory)
 * 0x00100000-0x0010FFFF: Kernel Base (1MB)
 * 0x00110000-0x001FFFFF: Kernel Heap/Data
 * 0xC0000000+: Higher Half Kernel (future)
 */

/* Physical load address (bootloader akan load kernel ke sini) */
KERNEL_PHYSICAL_BASE = 0x00100000;  /* 1MB */
KERNEL_VIRTUAL_BASE  = 0xC0000000;   /* 3GB - untuk higher-half nanti */

/* 
 * Symbol untuk mengetahui batas-batas memory
 * Digunakan oleh memory manager di src/memory.c
 */
PROVIDE(_memory_start = 0x00100000);
PROVIDE(_memory_end   = 0x00200000);   /* 2MB batas awal */
PROVIDE(_heap_start   = 0x00110000);   /* Heap mulai setelah kernel */
PROVIDE(_heap_end     = 0x001FFFFF);

/* 
 * SECTION LAYOUT - diurutkan berdasarkan urutan boot
 */
SECTIONS
{
    /* ====================================================
       BOOTLOADER SECTION (bootloader components)
       ==================================================== */
    . = KERNEL_PHYSICAL_BASE;
    
    /* Boot signature (harus di awal) */
    .bootsignature BLOCK(512) : ALIGN(512) {
        SHORT(0xAA55);                    /* Boot signature */
        LONG(0x1BADB002);                  /* Multiboot header (optional) */
        LONG(0x00000004);
    }
    
    /* Bootloader code (harus < 512 byte) */
    .boot BLOCK(512) : ALIGN(512) {
        boot/*(.text)                      /* boot.asm */
        boot/*(.text.*)
    }
    
    /* ====================================================
       KERNEL ENTRY SECTION (kernel_entry.asm)
       ==================================================== */
    .entry BLOCK(4K) : ALIGN(4K) {
        PROVIDE(_start = .);
        boot/kernel_entry.o(.text)        /* Entry point assembly */
        boot/kernel_entry.o(.data)
    }
    
    /* ====================================================
       KERNEL CORE SECTION (kernel/*.c)
       ==================================================== */
    .text BLOCK(4K) : ALIGN(4K) {
        /* Kernel utama */
        kernel/kernel.o(.text)            /* kernel.c - main kernel */
        kernel/loading.o(.text)           /* loading.c - loading screen */
        kernel/driver.o(.text)            /* driver.c - I/O helpers */
        
        /* Core libraries */
        src/string.o(.text)               /* string.c */
        src/io.o(.text)                   /* io.c */
        src/memory.o(.text)               /* memory.c */
        
        /* Filesystem */
        fs/fat12.o(.text)                 /* fat12.c */
        
        /* Kode lain yang mungkin ada */
        *(.text)
        *(.text.*)
    }
    
    /* ====================================================
       DRIVERS SECTION (drivers/*.c) - Diurutkan berdasarkan priority
       ==================================================== */
    .drivers BLOCK(4K) : ALIGN(4K) {
        /* 1. VGA driver pertama (harus ada untuk output) */
        drivers/vga.o(.text)              /* vga.c */
        
        /* 2. PIC (Programmable Interrupt Controller) */
        drivers/pic.o(.text)              /* pic.c */
        
        /* 3. Timer/PIT */
        drivers/timer.o(.text)            /* timer.c */
        
        /* 4. Keyboard */
        drivers/keyboard.o(.text)         /* keyboard.c */
        
        /* 5. Serial port */
        drivers/serial.o(.text)           /* serial.c */
        
        /* 6. ATA Disk driver */
        drivers/ata.o(.text)              /* ata.c */
        
        /* Driver lainnya */
        drivers/*(.text)
        drivers/*(.text.*)
    }
    
    /* ====================================================
       RODATA SECTION (read-only data)
       ==================================================== */
    .rodata BLOCK(4K) : ALIGN(4K) {
        /* Kernel strings */
        kernel/*(.rodata)
        kernel/*(.rodata.*)
        
        /* Driver strings dan constants */
        drivers/*(.rodata)
        drivers/*(.rodata.*)
        
        /* Filesystem constants */
        fs/*(.rodata)
        fs/*(.rodata.*)
        
        /* Library constants */
        src/*(.rodata)
        src/*(.rodata.*)
        
        *(.rodata)
        *(.rodata.*)
    }
    
    /* ====================================================
       DATA SECTION (initialized data)
       ==================================================== */
    .data BLOCK(4K) : ALIGN(4K) {
        /* Global initialized variables */
        kernel/*(.data)
        drivers/*(.data)
        fs/*(.data)
        src/*(.data)
        
        /* Static constructors (jika ada C++) */
        PROVIDE(__init_array_start = .);
        KEEP(*(.init_array))
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        PROVIDE(__init_array_end = .);
        
        *(.data)
        *(.data.*)
    }
    
    /* ====================================================
       BSS SECTION (uninitialized data - HARUS di-clearkan)
       ==================================================== */
    .bss BLOCK(4K) : ALIGN(4K) {
        /* Simbol untuk kernel clear BSS */
        PROVIDE(__bss_start = .);
        
        kernel/*(.bss)
        kernel/*(.bss.*)
        kernel/*(COMMON)
        
        drivers/*(.bss)
        drivers/*(.bss.*)
        drivers/*(COMMON)
        
        fs/*(.bss)
        fs/*(.bss.*)
        fs/*(COMMON)
        
        src/*(.bss)
        src/*(.bss.*)
        src/*(COMMON)
        
        /* Semua BSS lainnya */
        *(COMMON)
        *(.bss)
        *(.bss.*)
        
        PROVIDE(__bss_end = .);
    }
    
    /* ====================================================
       KERNEL STACK (16KB untuk kernel stack)
       ==================================================== */
    .stack BLOCK(4K) : ALIGN(4K) {
        PROVIDE(_kernel_stack_bottom = .);
        . += 16K;                         /* 16KB kernel stack */
        PROVIDE(_kernel_stack_top = .);
    }
    
    /* ====================================================
       HEAP SECTION (untuk dynamic allocation)
       ==================================================== */
    .heap BLOCK(4K) : ALIGN(4K) {
        PROVIDE(_heap_base = .);
        . = ALIGN(4K);
        PROVIDE(_heap_limit = . + 64K);   /* 64KB heap awal */
    }
    
    /* ====================================================
       KERNEL END MARKER
       ==================================================== */
    . = ALIGN(4K);
    PROVIDE(_kernel_end = .);
    
    /* ====================================================
       DISCARD SECTION (unused sections)
       ==================================================== */
    /DISCARD/ : {
        /* Debug sections */
        *(.comment)
        *(.note.*)
        *(.eh_frame)
        *(.eh_frame_hdr)
        
        /* GCC exception handling (tidak digunakan) */
        *(.gcc_except_table)
        *(.gcc_except_table.*)
    }
}

/*
 * Symbols yang diexport ke kernel C code:
 * 
 * 1. Di kernel.c, tambahkan:
 *    extern uint32_t __bss_start, __bss_end;
 *    extern uint32_t _kernel_stack_top;
 *    extern uint32_t _heap_base, _heap_limit;
 * 
 * 2. Di memory.c, gunakan _heap_base dan _heap_limit
 *    untuk memory manager.
 * 
 * 3. Di boot/kernel_entry.asm, set stack pointer ke
 *    _kernel_stack_top.
 */
