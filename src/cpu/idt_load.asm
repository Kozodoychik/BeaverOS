section .text
global idt_load
extern idt

idtr:
    dw 0x00
    dd idt

idt_load:
    mov ax, [esp + 4]
    mov [idtr], ax

    cli
    lidt [idtr]
    sti
    
    ret