/* ================================
   Linker Script - BloodG OS Kernel
   Safe & Stable OSDev Layout
   ================================ */

ENTRY(_start)

SECTIONS
{
    /* Kernel load address (1MB) */
    . = 0x00100000;

    /* ---------------- TEXT ---------------- */
    .text ALIGN(4K) :
    {
        KEEP(*(.multiboot))
        *(.text*)
    }

    /* ---------------- RODATA ---------------- */
    .rodata ALIGN(4K) :
    {
        *(.rodata*)
    }

    /* ---------------- DATA ---------------- */
    .data ALIGN(4K) :
    {
        *(.data*)
    }

    /* ---------------- BSS ---------------- */
    .bss ALIGN(4K) :
    {
        *(COMMON)
        *(.bss*)
    }

    /* ---------------- STACK ---------------- */
    . = ALIGN(16);
    stack_bottom = .;
    . += 0x4000; /* 16 KB stack */
    stack_top = .;
}
