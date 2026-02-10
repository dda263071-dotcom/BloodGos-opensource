; =========================
; BloodG Bootloader FIXED
; =========================

BITS 16
org 0x7C00

KERNEL_ADDR equ 0x100000

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov ax, 0x0003
    int 0x10

    mov si, msg_booting
    call print_string

    ; Load kernel (RAW BIN!)
    mov bx, KERNEL_ADDR >> 4
    mov es, bx
    xor bx, bx

    mov ah, 0x02
    mov al, 32            ; adjust kernel size
    mov ch, 0
    mov dh, 0
    mov cl, 2
    int 0x13
    jc disk_error

    call enable_a20
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEG:pmode


BITS 32
pmode:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x9FC00

    ; FAR JUMP KE KERNEL
    jmp CODE_SEG:KERNEL_ADDR
