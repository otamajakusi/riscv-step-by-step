srcs  = crt.S main.c syscall.S setup.c thread.c atomic.S
target = u.elf
include ../../../mk/common.mk
LDFLAGS += -L$(MAKEFILE_DIR) -Tu-va.ld
