#include <stdint.h>

typedef struct {
    uint16_t limit;
    uint16_t base;
    uint8_t base_m;
    uint8_t access;
    uint8_t limit_high : 4;
    uint8_t flags : 4;
    uint8_t base_h;
}__attribute__((packed)) gdt_entry;


gdt_entry gdt[4];

void gdt_init();