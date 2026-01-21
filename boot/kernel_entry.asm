; Kernel Entry Point with Filesystem Initialization
; Sets up stack, calls hardware init, then calls kmain()

BITS 32

global start
extern kmain
extern ata_init
extern fat12_init

start:
    ; Set up stack
    mov esp, kernel_stack + 0x4000
    
    ; Clear direction flag
    cld
    
    ; Initialize hardware
    call hardware_init
    
    ; Call main kernel function
    call kmain
    
    ; Halt if kmain returns
    cli
.hang:
    hlt
    jmp .hang

hardware_init:
    pusha
    
    ; Initialize ATA controller
    call ata_init
    
    ; Initialize FAT12 filesystem
    call fat12_init
    
    ; Note: These functions return bool, but we ignore for now
    ; In production, you'd want to check return values
    
    popa
    ret

section .bss
align 16
kernel_stack:
    resb 0x4000               ; 16KB stack
