; Kernel Entry Point
; Sets up stack and calls kmain()

BITS 32

global start
extern kmain

start:
    ; Set up stack
    mov esp, kernel_stack + 0x4000
    
    ; Clear direction flag
    cld
    
    ; Call main kernel function
    call kmain
    
    ; Halt if kmain returns (should not happen)
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 16
kernel_stack:
    resb 0x4000               ; 16KB stack
