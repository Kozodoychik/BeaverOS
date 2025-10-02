#include <drivers/vga.h>
#include <drivers/io.h>
#include <string.h>


vga_char* vga_buffer = (vga_char*)0xb8000;
uint8_t vga_attribute = 0x07;

void vga_set_cursor_pos(int x, int y) {
    uint16_t pos = y * VGA_TEXT_WIDTH + x;

    vga_set_cursor_pos_offset(pos);
}

void vga_set_cursor_pos_offset(uint16_t offset) {
    outb(0x3d4, 0x0f);
    outb(0x3d5, (uint8_t)(offset & 0xff));

    outb(0x3d4, 0x0e);
    outb(0x3d5, (uint8_t)(offset >> 8));
};

uint16_t vga_get_cursor_pos() {
    uint16_t value = 0;

    outb(0x3d4, 0x0f);
    value |= inb(0x3d5);

    outb(0x3d4, 0x0e);
    value |= ((uint16_t)inb(0x3d5)) << 8;

    return value;
}

void vga_putc(char c) {
    uint16_t pos = vga_get_cursor_pos();
    
    vga_buffer[pos].character = c;
    vga_buffer[pos].attribute = vga_attribute;
    
    pos++;
    vga_set_cursor_pos_offset(pos);
}

void vga_cls() {
    vga_set_cursor_pos_offset(0);

    for (int y=0;y<VGA_TEXT_HEIGHT;y++) {
        for (int x=0;x<VGA_TEXT_WIDTH;x++) {
            int offset = y * VGA_TEXT_WIDTH + x;
            vga_buffer[offset].character = ' ';
            vga_buffer[offset].attribute = vga_attribute;
        }
    }
}

void vga_scroll() {
    uint16_t pos = vga_get_cursor_pos();
    memcpy((uint8_t*)vga_buffer, (uint8_t*)(vga_buffer+VGA_TEXT_WIDTH), VGA_TEXT_WIDTH * (VGA_TEXT_HEIGHT - 1) * 2);
    
    for (int x = 0; x < VGA_TEXT_WIDTH; x++) {
        vga_buffer[x + (VGA_TEXT_WIDTH * (VGA_TEXT_HEIGHT - 1))].character = ' ';
        vga_buffer[x + (VGA_TEXT_WIDTH * (VGA_TEXT_HEIGHT - 1))].attribute = vga_attribute;
    }

    vga_set_cursor_pos_offset(pos - VGA_TEXT_WIDTH);
}