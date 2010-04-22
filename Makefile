CC=gcc
LD=ld
ASFLAGS=-I include -m32
CFLAGS=-fno-builtin -fno-stack-protector -Wall -I include -m32

KERNELOBJS = kernel/kernel.o kernel/start.o kernel/i8259.o \
					kernel/protect.o kernel/global.o kernel/main.o \
					kernel/proc.o kernel/clock.o \
					kernel/syscall.o kernel/lvl0call.o \
					kernel/system.o kernel/idle.o
DRIVERSOBJS = drivers/keyboard.o
LIBOBJS = lib/string.o lib/klib.o lib/kliba.o lib/misc.o lib/timer.o
KERNELSTAMP=kernel/.STAMP
OBJS=$(KERNELOBJS) $(LIBOBJS) $(DRIVERSOBJS)
TARGETS=buildkernel floppy.img

all: $(TARGETS)

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

buildkernel: $(KERNELSTAMP) kernel/KERNEL.BIN

$(KERNELSTAMP): include/*.h
	@$(MAKE) cleankernel;\
	touch $(KERNELSTAMP)

kernel/KERNEL.BIN: $(OBJS) kernel/kernel.ld
	$(LD) $(OBJS) -o $@ -T kernel/kernel.ld -m elf_i386

copy: floppy.img boot/LOADER.BIN buildkernel 
	@-mkdir -p /tmp/floppy;\
	sudo mount -o loop floppy.img /tmp/floppy -o fat=12;\
	sudo cp boot/LOADER.BIN kernel/KERNEL.BIN /tmp/floppy/;\
	sudo umount /tmp/floppy;\
	rmdir /tmp/floppy;\
	echo "###copy complete###"

cleankernel:
	@-rm -f $(OBJS) kernel/KERNEL.BIN $(KERNELSTAMP)

clean:
	@$(MAKE) cleankernel
	@-rm -f floppy.img KERNELASM

