all:
	-i686-elf-as boot.s -o boot.o
	-i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	-i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc
	-cp myos.bin isodir/boot/grub/

iso:
	-grub-mkrescue -o myos.iso isodir

.PHONY: clean
clean:
	-rm *.o $(objects)

cleanex:
	-rm *.iso
	-rm *.bin
	-rm isodir/boot/grub/myos.bin