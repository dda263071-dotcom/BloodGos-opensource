; Error Handler - BloodG Not Found
; Called when kernel validation fails

BITS 32

global false_error

false_error:
    ; Set video mode
    mov edi, 0xB8000
    mov ah, 0x4F            ; White text on red background
    
    ; Clear screen with red
    mov ecx, 80*25
    mov al, ' '
    rep stosw
    
    ; Display error message
    mov edi, 0xB8000 + (10 * 80 + 30) * 2
    mov esi, error_msg
.display_loop:
    lodsb
    test al, al
    jz .done
    stosw
    inc edi
    inc edi
    jmp .display_loop
.done:
    cli
.hang:
    hlt
    jmp .hang

section .data
error_msg db "ERROR: BloodG OS kernel not found!", 0
