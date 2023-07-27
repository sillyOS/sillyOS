CFLAGS="-O2 -I src -nostdlib -nostartfiles -ffreestanding -fno-stack-protector -nodefaultlibs -fno-pie -m32 -fno-builtin -mno-red-zone"

mkdir -p tmp/vga/
mkdir -p tmp/memory/
mkdir -p tmp/interrupts/
mkdir -p tmp/keyboard/

nasm -f bin src/boot.asm -o tmp/boot.o
nasm -f elf32 src/kernel_entry.asm -o tmp/kernel_entry.o

gcc $CFLAGS src/kernel.c -c -o tmp/kernel.o
gcc $CFLAGS src/fpuinit.c -c -o tmp/fpuinit.o
gcc $CFLAGS src/cassert.c -c -o tmp/cassert.o
gcc $CFLAGS src/stdio.c -c -o tmp/stdio.o
gcc $CFLAGS src/vga/vga.c -c -o tmp/vga/vga.o
gcc $CFLAGS src/memory/memory.c -c -o tmp/memory/memory.o
gcc $CFLAGS src/hal.c -c -o tmp/hal.o
gcc $CFLAGS src/keyboard/keyboard.c -c -o tmp/keyboard/keyboard.o


ld -m elf_i386 -T linker.ld tmp/kernel_entry.o tmp/kernel.o tmp/fpuinit.o tmp/cassert.o tmp/stdio.o tmp/vga/vga.o tmp/memory/memory.o  tmp/hal.o tmp/keyboard/keyboard.o -o tmp/kernel.bin

# truncate -s 28672 tmp/kernel.bin
cat tmp/boot.o tmp/kernel.bin > tmp/os.bin

# -d guest_errors

cp tmp/os.bin .
qemu-system-x86_64 -serial file:text.txt -fda os.bin