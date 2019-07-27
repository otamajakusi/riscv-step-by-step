srcs  = crt.S main.c syscall.S setup.c thread.c thread_example.c atomic.S atomic_test.c
target = u.elf
include ../../../mk/common.mk
LDFLAGS += -L$(MAKEFILE_DIR) -Tu-va.ld
