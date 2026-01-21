; Shutdown/Reboot Functions
; For QEMU and real hardware

BITS 32

global shutdown
global reboot

shutdown:
    ; Try ACPI shutdown first
    mov ax, 0x2000
    mov dx, 0x604
    out dx, ax
    
    ; Try Bochs shutdown
    mov dx, 0xB004
    mov ax, 0x2000
    out dx, ax
    
    ; Try VirtualBox shutdown
    mov dx, 0x4004
    mov ax, 0x3400
    out dx, ax
    
    ; If still running, just halt
    cli
.halt_loop:
    hlt
    jmp .halt_loop

reboot:
    ; Try keyboard controller reboot
    mov al, 0xFE
    out 0x64, al
    
    ; Fallback: Triple fault
    cli
    lidt [0]
    int 0
