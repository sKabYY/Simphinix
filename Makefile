CC=gcc
LD=ld
ASFLAGS=-I include
CFLAGS=-c -fno-builtin -fno-stack-protector -Wall -I include

KERNELOBJS = kernel/kernel.o kernel/start.o kernel/i8259.o \
					kernel/protect.o kernel/global.o kernel/main.o \
					kernel/proc.o kernel/clock.o kernel/keyboard.o \
					kernel/syscall.o kernel/systask.o
LIBOBJS = lib/string.o lib/klib.o lib/kliba.o lib/misc.o
TARGETS = boot/LOADER.BIN kernel/KERNEL.BIN floppy.img
KERNELSTAMP = kernel/.STAMP

all: $(KERNELSTAMP) $(TARGETS)

KERNELASM: kernel/KERNEL.BIN
	echo "/*======HEADER======*/" > KERNELASM
	objdump -h kernel/KERNEL.BIN >> KERNELASM
	echo "\n\n/*======DISAMS======*/" >> KERNELASM
	objdump -S kernel/KERNEL.BIN >> KERNELASM

floppy.img: boot/boot.bin
	@dd if=boot/boot.bin of=floppy.img bs=512 count=1
	@dd if=/dev/zero of=floppy.img skip=1 seek=1 bs=512 count=2879

boot/LOADER.BIN boot/boot.bin: boot/boot.S boot/loader.S \
			boot/boot.ld boot/lib.h boot/include/*.h
	$(MAKE) -C boot

kernel/KERNEL.BIN: $(KERNELOBJS) $(LIBOBJS) kernel/kernel.ld
	$(LD) $(KERNELOBJS) $(LIBOBJS) \
			-o $@ -T kernel/kernel.ld

$(KERNELSTAMP): include/*.h
	@$(MAKE) cleankernel;\
	$(MAKE) kernel/KERNEL.BIN;\
	touch $(KERNELSTAMP)

copy: floppy.img boot/LOADER.BIN kernel/KERNEL.BIN
	@-mkdir -p /tmp/floppy;\
	sudo mount -o loop floppy.img /tmp/floppy -o fat=12;\
	sudo cp boot/LOADER.BIN kernel/KERNEL.BIN /tmp/floppy/;\
	sudo umount /tmp/floppy;\
	rmdir /tmp/floppy;\
	echo "###copy complete###"

cleankernel:
	@-rm -f $(KERNELOBJS) $(LIBOBJS) kernel/KERNEL.BIN

clean:
	@$(MAKE) cleankernel
	@-rm -f floppy.img KERNELASM

