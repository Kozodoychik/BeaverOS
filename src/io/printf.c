#include <io/printf.h>
#include <drivers/vga.h>
#include <stdbool.h>

char hex_digits[16] = "0123456789abcdef";

void printk(char* string, ...) {
    uintptr_t arg_ptr = (uint32_t)(&string) + sizeof(char*);

    for (char* c = string; *c != 0; c++) {
        uint16_t pos = vga_get_cursor_pos();
        
        uint8_t x = pos % VGA_TEXT_WIDTH;
        uint8_t y = (pos / VGA_TEXT_WIDTH) + 1;

        switch (*c) {
            case '\n': {
                vga_set_cursor_pos(0, y);
                if (y >= VGA_TEXT_HEIGHT) {
                    vga_scroll();
                }
                break;
            }
            case '%': {
                switch (*(c+1)) {
                    case 'x': {
                        print_hex(*(uint32_t*)arg_ptr);
                        arg_ptr += sizeof(uint32_t*);
                        break;
                    }
                    case 's': {
                        printk(*(char**)(arg_ptr));
                        arg_ptr += sizeof(char*);
                        break;
                    }
                }
                c += 1;
                break;
            }
            default: {
                vga_putc(*c);
            }
        }
        if (y >= VGA_TEXT_HEIGHT && x >= VGA_TEXT_WIDTH-1) {
            vga_set_cursor_pos(0, y);
            vga_scroll();
        }
    }
}

void print_hex(uint32_t value) {

    bool print = false;

    if (value == 0) {
        vga_putc(hex_digits[0]);
        return;
    }
    int shift = 28;
    while (shift >= 0) {
        uint8_t index = (value >> shift) & 0xf;

        if (index > 0) print = true;
        if (print)
            vga_putc(hex_digits[index]);
        shift -= 4;
    }

    //printk(str);
}