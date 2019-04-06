srcs  = crt.S main.c setup.c elfldr.c vm.c plic.c
target = m.elf
objs = u.elf.o u-va.elf.o
include ../../mk/common.mk
include ../../mk/m.mk

u.elf:
	$(MAKE) -C ../u
	cp ../u/$@ .

u-va.elf:
	$(MAKE) -C ../u-va
	cp ../u-va/$@ .

u.elf.o: u.elf
	$(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $< $@

u-va.elf.o: u-va.elf
	$(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $< $@

