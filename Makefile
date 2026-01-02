# BloodOS Build System
AS = nasm
CC = i686-elf-gcc
LD = i686-elf-ld
CFLAGS = -ffreestanding -O2 -Wall -Wextra -fno-builtin -nostdlib
LDFLAGS = -T linker.ld -nostdlib

OBJS = kernel_entry.o kernel.o

all: bloodos.img

bloodos.img: boot.bin kernel.bin
	dd if=/dev/zero of=bloodos.img bs=512 count=2880
	dd if=boot.bin of=bloodos.img conv=notrunc
	dd if=kernel.bin of=bloodos.img bs=512 seek=1 conv=notrunc

boot.bin: boot.asm
	$(AS) -f bin boot.asm -o boot.bin

kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o kernel.bin $(OBJS)

kernel_entry.o: kernel_entry.asm
	$(AS) -f elf32 kernel_entry.asm -o kernel_entry.o

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

clean:
	rm -f *.o *.bin *.img

run: bloodos.img
	qemu-system-x86_64 -drive format=raw,file=bloodos.img

run-hdd: bloodos.img
	qemu-system-x86_64 -hda bloodos.img

.PHONY: all clean run run-hdd
