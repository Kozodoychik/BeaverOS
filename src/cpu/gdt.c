#include <cpu/gdt.h>
#include <string.h>


extern void gdt_load(uint16_t size);

void gdt_init() {
    memset((uint8_t*)gdt, 0, sizeof(gdt));

    asm volatile ("cli");

    gdt[1].limit = 0xffff;
    gdt[1].limit_high = 0xf;
    gdt[1].flags = 0x0c;
    gdt[1].access = 0x9a;

    gdt[2].limit = 0xffff;
    gdt[2].limit_high = 0x0f;
    gdt[2].flags = 0x0c;
    gdt[2].access = 0x92;

    gdt_load((uint16_t)(sizeof(gdt) - 1));
}