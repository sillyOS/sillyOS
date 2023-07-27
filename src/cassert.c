#include "cassert.h"

extern void printf(const char* fmt, ...);
void assert(char e, const char* s)
{
    if (e == 1)
    {
        printf("Assertion fail: %s", s);

_halt:
        __asm__ __volatile__("cli");
        __asm__ __volatile__("hlt");

        for (;;) goto _halt;
    }
}
