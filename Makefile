VERSION=0.0.1
BUILD_DATE=$(shell date +%d\ %b\ %Y\ %T\ %Z)

OUTPUT=out/kernel.bin

OBJ=obj

CC=i686-linux-gnu-gcc
CFLAGS=-nostdlib -O2 -Wall -Wextra -lgcc -ffreestanding -Iincl -DBUILD_DATE='"$(BUILD_DATE)"' -g

AS=nasm
ASFLAGS=-f elf

LD=i686-linux-gnu-gcc
LDFLAGS=-Tlink.ld -o $(OUTPUT) -ffreestanding -O2 -nostdlib -lgcc

ASFILES=$(shell find src -type f -name *.asm)
CFILES=$(shell find src -type f -name *.c)
COBJECTS=$(CFILES:src/%.c=obj/%.o)
ASOBJECTS=$(ASFILES:src/%.asm=obj/%.o)


all: clean $(ASOBJECTS) $(COBJECTS) $(OUTPUT) iso

obj/%.o: src/%.asm
	$(AS) $< -o obj/$(notdir $@) $(ASFLAGS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o obj/$(notdir $@)

$(OUTPUT):
	$(LD) $(LDFLAGS) obj/*.o

clean:
	rm -f obj/*.o
	rm -f out/*
	rm -rf iso

run:
	qemu-system-i386 -kernel $(OUTPUT) -d int -no-reboot -nic model=pcnet

run-iso:
	qemu-system-i386 -cdrom out/os.iso -d int -no-reboot -nic model=pcnet

run-gdb:
	qemu-system-i386 -kernel $(OUTPUT) -d int -s -S -nic model=pcnet

iso:
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $(OUTPUT) iso/boot/kernel
	echo 'menuentry "BeaverOS" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/kernel' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output out/os.iso iso
	rm -rf iso
