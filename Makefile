PREFIX = i686-elf
CC = $(PREFIX)-gcc
LD = $(PREFIX)-ld

CFLAGS = -ffreestanding -O2 -Wall -Wextra -std=gnu99
LDFLAGS = -T kernel/linker.ld

all: iso

build:
	mkdir -p build

build/boot.o: boot/boot.asm | build
	nasm -f elf32 boot/boot.asm -o build/boot.o

build/kernel.o: kernel/kernel.c | build
	$(CC) $(CFLAGS) -c kernel/kernel.c -o build/kernel.o

build/screen.o: kernel/screen.c | build
	$(CC) $(CFLAGS) -c kernel/screen.c -o build/screen.o

kernel.bin: build/boot.o build/kernel.o build/screen.o
	$(CC) -ffreestanding -nostdlib $(LDFLAGS) -o build/kernel.bin build/boot.o build/kernel.o build/screen.o -lgcc

iso: kernel.bin
	mkdir -p iso/boot/grub
	cp build/kernel.bin iso/boot/kernel.bin
	cp boot/grub/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o NanoVMM.iso iso

run: iso
	qemu-system-i386 -cdrom NanoVMM.iso

clean:
	rm -rf build NanoVMM.iso
