#include <stdint.h>

#define VGA_TEXT_WIDTH 80
#define VGA_TEXT_HEIGHT 25

typedef struct {
    char character;
    uint8_t attribute;
} vga_char;

vga_char* vga_buffer;
uint8_t vga_attribute;

void vga_set_cursor_pos(int x, int y);
void vga_set_cursor_pos_offset(uint16_t offset);
uint16_t vga_get_cursor_pos(); // Возвращает СМЕЩЕНИЕ (y * VGA_TEXT_WIDTH + x)

void vga_putc(char c);
void vga_cls();
void vga_scroll();