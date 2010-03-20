OBJCOPY=objcopy

TRIM_FLAGS=-R .pdr -R .comment -R .note -S -O binary

all: boot.bin LOADER.BIN

boot.bin: boot.S lib.h include/fat12hdr.h include/load.h boot.ld
	$(CC) -c boot.S
	$(LD) boot.o -o boot.elf -T boot.ld
	$(OBJCOPY) $(TRIM_FLAGS) boot.elf $@

LOADER.BIN: loader.S lib.h loader.ld
	$(CC) -c loader.S
	$(LD) loader.o -o loader.elf -T  loader.ld
	$(OBJCOPY) $(TRIM_FLAGS) loader.elf $@

clean:
	@rm -f *.o *.elf boot.bin LOADER.BIN

