srcs  = crt.S main.c setup.c syscall.S
target = u-va.elf
VPATH=../u
include ../../mk/common.mk
LDFLAGS += -L$(MAKEFILE_DIR) -Tu-va.ld
