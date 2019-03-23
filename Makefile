CC = riscv32-unknown-elf-gcc
LD = riscv32-unknown-elf-gcc

subdirs = src
.PHONY: all $(subdirs)

all: $(subdirs)
clean: $(subdirs)

$(subdirs):
	$(MAKE) -C $@ CC=$(CC) $(MAKECMDGOALS)

