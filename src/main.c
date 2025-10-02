#define KERNEL_VERSION "0.0.1"

#include <stdint.h>

#include <drivers/vga.h>
#include <drivers/pci.h>
#include <drivers/am79c970a.h>
#include <io/printf.h>
#include <cpu/gdt.h>
#include <cpu/interrupts.h>
#include <memory/malloc.h>

void kmain(){
    vga_cls();

    gdt_init();
    interrupts_init();

    init_heap(0xa0000000, 0xff);

    printk("\nBeaverOS kernel version %s (build %s)\n\n", KERNEL_VERSION, BUILD_DATE);

    pci_print_devices();

    //pcnet_init();
    uint8_t* hui = (uint8_t*)malloc(16);
    uint8_t* hui2 = (uint8_t*)malloc(256);

    printk("0x%x, 0x%x", hui, hui2);

    while(1);
}