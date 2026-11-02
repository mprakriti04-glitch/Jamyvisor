PREFIX = i686-linux-gnu
CC = $(PREFIX)-gcc
LD = $(PREFIX)-ld

CFLAGS = -ffreestanding -fno-pie -O2 -Wall -Wextra -std=gnu99
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
build/cpu.o: kernel/cpu.c | build
	$(CC) $(CFLAGS) -c kernel/cpu.c -o build/cpu.o
kernel.bin: build/boot.o build/kernel.o build/screen.o build/cpu.o
	$(CC) -ffreestanding -fno-pie -no-pie -nostdlib $(LDFLAGS) -o build/kernel.bin build/boot.o build/kernel.o build/screen.o build/cpu.o -lgcc

iso: kernel.bin
	mkdir -p iso/boot/grub
	cp build/kernel.bin iso/boot/kernel.bin
	cp boot/grub/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o NanoVMM.iso iso

run:
	qemu-system-i386 -cdrom NanoVMM.iso -enable-kvm -cpu host

clean:
	rm -rf build NanoVMM.iso
