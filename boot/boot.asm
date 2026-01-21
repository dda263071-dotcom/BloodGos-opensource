; BloodG OS Bootloader - FAT12 Aware
; Loads kernel at 0x1000 with proper segment setup

BITS 16
org 0x7C00

start:
    cli                     ; Disable interrupts
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00          ; Stack grows downward

    ; Clear screen
    mov ax, 0x0003
    int 0x10

    ; Print boot message
    mov si, msg_booting
    call print_string

    ; Load kernel from disk (sectors 2-16)
    mov ah, 0x02            ; Read sectors
    mov al, 15              ; Sectors to read (kernel size)
    mov ch, 0               ; Cylinder 0
    mov dh, 0               ; Head 0
    mov cl, 2               ; Start from sector 2
    mov bx, 0x1000          ; Load kernel at 0x1000 (64KB)
    int 0x13
    jc disk_error

    ; Enable A20 line
    call enable_a20

    ; Load GDT
    lgdt [gdt_descriptor]

    ; Switch to protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Far jump to flush pipeline
    jmp CODE_SEG:protected_mode

BITS 32
protected_mode:
    ; Setup segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Setup stack
    mov esp, 0x90000        ; Stack at 576KB
    
    ; Call kernel entry
    jmp 0x1000

BITS 16
disk_error:
    mov si, msg_disk_error
    call print_string
.halt:
    hlt
    jmp .halt

enable_a20:
    ; Enable A20 via keyboard controller
    call a20wait
    mov al, 0xAD
    out 0x64, al
    call a20wait
    mov al, 0xD0
    out 0x64, al
    call a20wait2
    in al, 0x60
    push eax
    call a20wait
    mov al, 0xD1
    out 0x64, al
    call a20wait
    pop eax
    or al, 2
    out 0x60, al
    call a20wait
    mov al, 0xAE
    out 0x64, al
    call a20wait
    ret

a20wait:
    in al, 0x64
    test al, 2
    jnz a20wait
    ret

a20wait2:
    in al, 0x64
    test al, 1
    jz a20wait2
    ret

print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

; GDT
gdt_start:
    dq 0x0000000000000000    ; Null descriptor

gdt_code:
    dw 0xFFFF                ; Limit 0-15
    dw 0x0000                ; Base 0-15
    db 0x00                  ; Base 16-23
    db 10011010b             ; Access byte: Present, Ring 0, Code, Exec/Read
    db 11001111b             ; Granularity 4K, 32-bit, limit 16-19
    db 0x00                  ; Base 24-31

gdt_data:
    dw 0xFFFF                ; Limit 0-15
    dw 0x0000                ; Base 0-15
    db 0x00                  ; Base 16-23
    db 10010010b             ; Access byte: Present, Ring 0, Data, Read/Write
    db 11001111b             ; Granularity 4K, 32-bit, limit 16-19
    db 0x00                  ; Base 24-31

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Messages
msg_booting db "Booting BloodG OS...", 0x0D, 0x0A, 0
msg_disk_error db "Disk error! System halted.", 0

; Boot signature
times 510-($-$$) db 0
dw 0xAA55
