#include <cpu/interrupts.h>
#include <string.h>
#include <io/printf.h>
#include <drivers/io.h>

extern void idt_load(uint16_t size);

extern void isr0x00();
extern void isr0x06();
extern void isr0x0d();

extern void isr0x20();
extern void isr0x21();
extern void isr0x22();
extern void isr0x23();
extern void isr0x24();
extern void isr0x25();
extern void isr0x26();
extern void isr0x27();

extern void isr0x28();
extern void isr0x29();
extern void isr0x2a();
extern void isr0x2b();
extern void isr0x2c();
extern void isr0x2d();
extern void isr0x2e();
extern void isr0x2f();

void (*irq_handlers[16])(void);

char* fault_msgs[] = {
    [0x00] = "Division Error",
    [0x06] = "Invalid Opcode",
    [0x0d] = "General Protection Fault"
};

void irq_handler(uint16_t irq) {
    irq -= 0x20;
    if ((uint32_t)irq_handlers[irq] == 0) {
        printk("WARN: Trying to call an unregistered IRQ handler (0x%x)\n", irq);
        pic_eoi();
        return;
    }
    (*irq_handlers[irq])();
    pic_eoi();
}

void exception_handler(uint16_t i) {
    printk("\n!!!EXCEPTION!!!\nVector: 0x%x (%s)\n", i, fault_msgs[i]);

    asm volatile ("cli");
    asm volatile ("hlt");
}

void pic_remap() {
    outb(PIC_MASTER_COMMAND, 0x11);
    outb(PIC_SLAVE_COMMAND, 0x11);

    outb(PIC_MASTER_DATA, 0x20);
    outb(PIC_SLAVE_DATA, 0x28);

    outb(PIC_MASTER_DATA, 0x04);
    outb(PIC_SLAVE_DATA, 0x02);

    outb(PIC_MASTER_DATA, 0x01);
    outb(PIC_SLAVE_DATA, 0x01);

    outb(PIC_MASTER_DATA, 0x0);
    outb(PIC_SLAVE_DATA, 0x0);
}

void pic_set_mask(uint8_t line) {
    uint16_t port;
    uint8_t mask;

    if (line < 8) port = PIC_MASTER_DATA;
    else {
        port = PIC_SLAVE_DATA;
        line -= 8;
    }

    mask = inb(port) | ~(1 << line);
    outb(port, mask);
}

void pic_clear_mask(uint8_t line) {
    uint16_t port;
    uint8_t mask;

    if (line < 8) port = PIC_MASTER_DATA;
    else {
        port = PIC_SLAVE_DATA;
        line -= 8;
    }

    mask = inb(port) & ~(1 << line);
    outb(port, mask);
}

void pic_eoi() {
    outb(PIC_MASTER_COMMAND, 0x20);
    outb(PIC_SLAVE_COMMAND, 0x20);
}

void interrupts_init() {
    memset((uint8_t*)idt, 0, sizeof(idt));
    memset((uint8_t*)irq_handlers, 0, sizeof(irq_handlers));


    pic_remap();

    outb(PIC_MASTER_DATA, 0xfb);
    outb(PIC_SLAVE_DATA, 0xff);

    // Исключения

    interrupt_set_handler(0x00, INTERRUPT_TRAP_GATE, isr0x00);
    interrupt_set_handler(0x06, INTERRUPT_TRAP_GATE, isr0x06);
    interrupt_set_handler(0x0d, INTERRUPT_TRAP_GATE, isr0x0d);

    // IRQ

    interrupt_set_handler(0x20, INTERRUPT_TRAP_GATE, isr0x20);
    interrupt_set_handler(0x21, INTERRUPT_TRAP_GATE, isr0x21);
    interrupt_set_handler(0x22, INTERRUPT_TRAP_GATE, isr0x22);
    interrupt_set_handler(0x23, INTERRUPT_TRAP_GATE, isr0x23);
    interrupt_set_handler(0x24, INTERRUPT_TRAP_GATE, isr0x24);
    interrupt_set_handler(0x25, INTERRUPT_TRAP_GATE, isr0x25);
    interrupt_set_handler(0x26, INTERRUPT_TRAP_GATE, isr0x26);
    interrupt_set_handler(0x27, INTERRUPT_TRAP_GATE, isr0x27);
    
    interrupt_set_handler(0x28, INTERRUPT_TRAP_GATE, isr0x28);
    interrupt_set_handler(0x29, INTERRUPT_TRAP_GATE, isr0x29);
    interrupt_set_handler(0x2a, INTERRUPT_TRAP_GATE, isr0x2a);
    interrupt_set_handler(0x2b, INTERRUPT_TRAP_GATE, isr0x2b);
    interrupt_set_handler(0x2c, INTERRUPT_TRAP_GATE, isr0x2c);
    interrupt_set_handler(0x2d, INTERRUPT_TRAP_GATE, isr0x2d);
    interrupt_set_handler(0x2e, INTERRUPT_TRAP_GATE, isr0x2e);
    interrupt_set_handler(0x2f, INTERRUPT_TRAP_GATE, isr0x2f);
    
    idt_load(sizeof(idt));
}

void interrupt_set_handler(int i, uint8_t type, void* handler) {
    idt[i].offset_low = (uint16_t)((uint32_t)handler & 0xffff);
    idt[i].offset_high = (uint16_t)((uint32_t)handler >> 16);
    idt[i].segment = 0x08;
    idt[i].attributes = type | (1 << 7);
}

void irq_register(int i, void* handler) {
    irq_handlers[i] = handler;
    pic_clear_mask(i);
}