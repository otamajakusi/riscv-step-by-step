srcs  = main.c elfldr.c
target = m.elf
objs = u.elf.o
include ../../mk/common.mk
include ../../mk/m.mk

u.elf:
	$(MAKE) -C ../u
	cp ../u/$@ .

u.elf.o: u.elf
	$(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $< $@

