srcs  = main.c
target = m.elf
subdirs = hifive1-revb
include ../../mk/subdirs.mk
include ../../mk/common.mk
include ../../mk/m.mk
LDFLAGS += $(MAKEFILE_DIR)../riscv-probe/build/obj/rv32imac/env/virt/crt.o \
           $(MAKEFILE_DIR)../riscv-probe/build/obj/rv32imac/env/virt/setup.o
