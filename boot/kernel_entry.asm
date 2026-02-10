; ===============================
; Kernel Entry Point (FIXED)
; ===============================

BITS 32

section .text
global _start

extern kmain
extern ata_init
extern fat12_init

_start:
    ; Setup stack (top-down)
    mov esp, kernel_stack + STACK_SIZE

    ; Clear direction flag
    cld

    ; Init hardware
    call hardware_init

    ; Enter kernel main
    call kmain

    ; If kmain returns → halt forever
    cli
.hang:
    hlt
    jmp .hang


hardware_init:
    pushad

    call ata_init
    call fat12_init

    popad
    ret


; ===============================
; Stack
; ===============================
section .bss
align 16

STACK_SIZE equ 0x4000    ; 16 KB

kernel_stack:
    resb STACK_SIZE
