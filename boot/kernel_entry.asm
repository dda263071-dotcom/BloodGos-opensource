BITS 32
section .text.start
global start

extern kmain

start:
    mov esp, kernel_stack + 0x4000
    cld
    call kmain

.hang:
    cli
    hlt
    jmp .hang

section .bss
align 16
kernel_stack:
    resb 0x4000
