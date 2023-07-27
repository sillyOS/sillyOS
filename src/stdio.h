#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include "stdarg.h"

#ifndef size_t
typedef unsigned int size_t;
#endif

void putc(char c);
void puts(const char* s);

void put_hex_digit(uint8_t digit);
void put_hex_byte(uint8_t byte);
void put_hex_word(uint16_t word);
void put_hex_dword(uint32_t dword);

void put_dec(int num);
void printf(const char* format, ...);
void _Stdio_Set_Line(int line);

#endif