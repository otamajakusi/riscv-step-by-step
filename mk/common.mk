MAKEFILE_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

CC = riscv32-unknown-elf-gcc
LD = riscv32-unknown-elf-gcc
OBJCOPY = riscv32-unknown-elf-objcopy

CFLAGS = -mcmodel=medany -march=rv32imac -mabi=ilp32 \
	     -Wall -Wextra -Wshadow -Werror -ffunction-sections -fdata-sections -Os
LDFLAGS = -L$(MAKEFILE_DIR)../riscv-probe/build/lib/rv32imac -lfemto \
		  -Wl,--gc-sections -Wl,--nmagic -nostartfiles -nostdlib -nostdinc -static
INCLUDE = -I$(MAKEFILE_DIR)../riscv-probe/libfemto/include

objs += $(patsubst %S,%o,$(filter %.S,$(srcs)))
objs += $(patsubst %c,%o,$(filter %.c,$(srcs)))
deps  = $(patsubst %c,%d,$(filter %.c,$(srcs)))
deps += $(patsubst %S,%d,$(filter %.S,$(srcs)))

.PHONY: all
all: $(objs) $(target) $(loader)

%.elf : $(objs)
	$(CC) $(LDFLAGS) $^ -lfemto -o $@

%.o : %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $< -MMD -MP

-include $(deps)

.PHONY: clean
clean:
	$(RM) $(objs) $(target) $(deps) $(loader)
