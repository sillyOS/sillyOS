section .text
global start
[bits 32]
[extern kernel_main]

start:

mov esp, .stack_top
push eax

call kernel_main

jmp $

section .bss
.stack_bottom:
    resb 8192
.stack_top: