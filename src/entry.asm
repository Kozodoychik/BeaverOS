section .text
global _start
extern kmain
bits 32
_start:
    mov esp, bss_end

    push eax
    push ebx
    
    call kmain

    cli
    hlt

section .bss
bss_start:
resb 1024 * 1024
bss_end: