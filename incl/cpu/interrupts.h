#include <stdint.h>

#define INTERRUPT_GATE 0x0e
#define INTERRUPT_TRAP_GATE 0x0f

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1

//#define ISR_ROUTINE(%n, handler) __attribute__((noreturn)) void %n{ handler(n); asm volatile("iret"); }

typedef struct {
    uint16_t offset_low;
    uint16_t segment;
    uint8_t reserved;
    uint8_t attributes;
    uint16_t offset_high;
} idt_entry;

idt_entry idt[256];

void pic_set_mask(uint8_t line);
void pic_clear_mask(uint8_t line);
void pic_eoi();
void interrupts_init();
void interrupt_set_handler(int i, uint8_t type, void* handler);
void irq_register(int i, void* handler);