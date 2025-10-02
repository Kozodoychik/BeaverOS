extern exception_handler
extern irq_handler

extern page_fault_handler

extern printk

%macro isr 2
isr%1:
    pusha
    mov ax, %1
    push ax
    call %2
    pop ax
    popa
    sti
    iret
global isr%1
%endmacro

isr 0x00, exception_handler
isr 0x06, exception_handler
isr 0x0d, exception_handler

isr 0x20, irq_handler
isr 0x21, irq_handler
isr 0x22, irq_handler
isr 0x23, irq_handler
isr 0x24, irq_handler
isr 0x25, irq_handler
isr 0x26, irq_handler
isr 0x27, irq_handler

isr 0x28, irq_handler
isr 0x29, irq_handler
isr 0x2a, irq_handler
isr 0x2b, irq_handler
isr 0x2c, irq_handler
isr 0x2d, irq_handler
isr 0x2e, irq_handler
isr 0x2f, irq_handler

isr0x0e:
    mov ecx, [esp]
    mov edx, [esp+4]

    mov eax, cr3
    push eax
    mov eax, cr2
    push eax
    push ecx
    push edx
    mov eax, page_fault_msg
    push eax
    
    call printk

    cli
    hlt
global isr0x0e

page_fault_msg: db "!!!PAGE FAULT!!! (ip=0x%x, code=0x%x, vmem=0x%x, pd=0x%x)", 0x00