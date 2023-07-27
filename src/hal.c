#include "hal.h"
// #include "interrupts/interrupts.h"

void HAL_Init()
{
    printf("Initializing Floating Point Unit...\n");
    fpu_init();
    printf("Floating Point Unit has been initialized!\n");

    //printf("Initializing IDT...\n");
    //interrupts_install_idt();
    //printf("Initialized IDT!\n");
}