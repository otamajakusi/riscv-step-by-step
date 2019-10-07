srcs  = crt.S main.c
target = u.elf
include ../../../mk/common.mk
LDFLAGS += -L$(MAKEFILE_DIR) -Tu.ld
LDFLAGS += $(MAKEFILE_DIR)../riscv-probe/build/obj/rv32imac/env/virt/setup.o
