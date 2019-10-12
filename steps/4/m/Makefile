srcs  = main.c elfldr.c
target = m.elf
objs = u.elf.o
include ../../../mk/common.mk
include ../../../mk/m.mk
LDFLAGS += $(MAKEFILE_DIR)../riscv-probe/build/obj/rv32imac/env/virt/crt.o \
           $(MAKEFILE_DIR)../riscv-probe/build/obj/rv32imac/env/virt/setup.o

../u/u.elf:
	$(MAKE) -C ../u

u.elf.o: ../u/u.elf
	$(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $< $@ \
		--redefine-sym _binary____u_u_elf_start=u_elf_start \
		--redefine-sym _binary____u_u_elf_end=u_elf_end \
		--redefine-sym _binary____u_u_elf_size=u_elf_size
