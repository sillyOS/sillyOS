#include <stdint.h>
#include "vga/vga.h"
#include "io/io.h"
#include "stdio.h"

void clear_screen()
{
    uint16_t* framebuffer = (uint16_t*)0xB8000;  // Address of the framebuffer
    uint8_t attributeByte = 0x07;  // Attribute byte (white on black)
    uint16_t blank = 0x20 | (attributeByte << 8);  // Space character with attribute byte

    // Fill the entire screen with blank spaces
    for (int i = 0; i < 80 * 25; i++) {
        framebuffer[i] = blank;
    }

    _Stdio_Set_Line(0);
}
void disable_cursor()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}