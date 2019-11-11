vpath % ../
srcs  = main.c setup.c
target = m.elf loader loader.hex
include ../../../mk/common.mk
include ../../../mk/m.mk
include ../../../mk/loader.mk
LDFLAGS += $(MAKEFILE_DIR)../riscv-probe/build/obj/rv32imac/env/virt/crt.o
