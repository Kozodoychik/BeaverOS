section .text
global gdt_load
extern gdt

gdtr:
    dw 0x00
    dd gdt

gdt_load:
    mov ax, [esp + 4]
    mov [gdtr], ax

    lgdt [gdtr]

    jmp 0x08:gdt_loaded

gdt_loaded:
    mov ax, 0x10
    mov es, ax
    mov ds, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax

    ret
