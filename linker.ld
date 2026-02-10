/* =========================
   linker.ld - BloodGos
   Safe & Standard OSDev
   ========================= */

OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(_start)

/* Kernel load address */
KERNEL_BASE = 0x00100000;

SECTIONS
{
    /* Kernel dimulai di 1MB */
    . = KERNEL_BASE;

    /* ---------- TEXT ---------- */
    .text ALIGN(4K) :
    {
        *(.multiboot)      /* kalau pakai multiboot */
        *(.text*)
    }

    /* ---------- RODATA ---------- */
    .rodata ALIGN(4K) :
    {
        *(.rodata*)
    }

    /* ---------- DATA ---------- */
    .data ALIGN(4K) :
    {
        *(.data*)
    }

    /* ---------- BSS ---------- */
    .bss ALIGN(4K) :
    {
        *(COMMON)
        *(.bss*)
    }

    /* ---------- END SYMBOL ---------- */
    end = .;
}
