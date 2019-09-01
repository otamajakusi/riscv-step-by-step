srcs  = crt.S main.c syscall.S setup.c thread.c mutex.c condvar.c atomic.S atomic_test.c
target = u.elf
include ../../../mk/common.mk
LDFLAGS += -L$(MAKEFILE_DIR) -Tu-va.ld
