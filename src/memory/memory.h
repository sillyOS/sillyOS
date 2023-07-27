#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

// Function to set a block of memory to a specific value
void* memset(void* dest, int value, size_t n);
int memcmp(const void* ptr1, const void* ptr2, size_t num);
void* memcpy(void* dest, const void* src, size_t n);
int strncmp(const char* str1, const char* str2, size_t n);

#endif // MEMORY_H
