#include <stdint.h>
#include "stdio.h"
#include "vga/vga.h"

volatile uint16_t* fb = (volatile uint16_t*)0xB8000;
int current_line = 0;
const int MAX_COLUMNS = 80;
const int MAX_LINES = 25;

void _Stdio_Set_Line(int line)
{
    current_line = line;
}

void putc(char c)
{
    if (c == '\n') {
        // Handle newline character
        // Move the pointer to the start of the next line
        current_line++;
        if (current_line >= MAX_LINES) {
            // If we've reached the last line, reset to the top
            current_line = 0;
            fb = (volatile uint16_t*)0xB8000;
            clear_screen();
        } else {
            // Move to the start of the next line
            fb = (volatile uint16_t*)(0xB8000 + (MAX_COLUMNS * current_line * 2));
        }
    } else {
        if ((uint32_t)fb >= (0xB8000 + (MAX_COLUMNS * MAX_LINES * 2)))
        {
            // If we have reached the end of the buffer, reset to the top
            fb = (volatile uint16_t*)0xB8000;
            current_line = 0;
        }

        *fb = (0x0F << 8) | c;
        fb++;
    }
}
void puts(const char* s)
{
    while (*s != '\0')
    {
        putc(*s);
        s++;
    }
}

void put_hex_digit(uint8_t digit) {
    char hex_digit;
    if (digit < 10)
        hex_digit = '0' + digit;
    else
        hex_digit = 'A' + (digit - 10);
    putc(hex_digit);
}
void put_hex_byte(uint8_t byte) {
    put_hex_digit(byte >> 4);  // Print the most significant digit
    put_hex_digit(byte & 0x0F);  // Print the least significant digit
}
void put_hex_word(uint16_t word) {
    put_hex_byte(word >> 8);  // Print the most significant byte
    put_hex_byte(word & 0xFF);  // Print the least significant byte
}
void put_hex_dword(uint32_t dword) {
    put_hex_word(dword >> 16);  // Print the most significant word
    put_hex_word(dword & 0xFFFF);  // Print the least significant word
}

void put_dec(int num) {
    if (num < 0) {
        putc('-');
        num = -num;
    }

    if (num >= 10) {
        put_dec(num / 10);
    }

    putc('0' + (num % 10));
}

void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++; // Move to the next character after '%'

            switch (*format) {
                case 'c': {
                    char c = va_arg(args, int);
                    putc(c);
                    break;
                }
                case 's': {
                    char* str = va_arg(args, char*);
                    puts(str);
                    break;
                }
                case 'd': {
                    int num = va_arg(args, int);
                    put_dec(num);
                    break;
                }
                case 'u': {
                    unsigned int num = va_arg(args, unsigned int);
                    put_dec(num);
                    break;
                }
                case 'x': {
                    unsigned int num = va_arg(args, unsigned int);
                    put_hex_dword(num);
                    break;
                }
                default:
                    // Unsupported format specifier; ignore and continue
                    putc(*format);
                    break;
            }
        } else {
            putc(*format);
        }

        format++;
    }

    va_end(args);
}
