srcs  = crt.S main.c syscall.S setup.c
target = u.elf
include ../../../mk/common.mk
LDFLAGS += -T./u-va-4mpage.ld
vpath %.c ../../7/u
vpath %.S ../../7/u
INCLUDE += -I../../7/u
