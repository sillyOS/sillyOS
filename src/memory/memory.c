#include "memory.h"

// Function to set a block of memory to a specific value
void* memset(void* dest, int value, size_t n) {
    uint8_t* dest_ptr = (uint8_t*)dest;
    uint8_t byte_value = (uint8_t)value;

    while (n > 0) {
        *dest_ptr++ = byte_value;
        n--;
    }

    return dest;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num)
{
    const unsigned char* p1 = (const unsigned char*)ptr1;
    const unsigned char* p2 = (const unsigned char*)ptr2;

    for (size_t i = 0; i < num; ++i) {
        if (p1[i] != p2[i]) {
            return (p1[i] < p2[i]) ? -1 : 1;
        }
    }

    return 0;
}

void* memcpy(void* dest, const void* src, size_t n)
{
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;

    // Copy the data byte by byte
    for (size_t i = 0; i < n; ++i)
    {
        d[i] = s[i];
    }

    return dest;
}

int strncmp(const char* str1, const char* str2, size_t n)
{
    while (n > 0 && *str1 && (*str1 == *str2)) {
        str1++;
        str2++;
        n--;
    }

    if (n == 0) {
        return 0; // The first n characters are equal
    } else {
        return *(unsigned char*)str1 - *(unsigned char*)str2;
        // Casting to unsigned char is necessary to handle negative values correctly
    }
}

