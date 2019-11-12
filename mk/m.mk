LDFLAGS += -L$(MAKEFILE_DIR)../riscv-probe/env/virt -Tdefault.lds
QEMU_VERSION = $(shell qemu-system-riscv32 -version | awk /^QEMU/)
ifneq ($(QEMU_VERSION), QEMU emulator version 3.1.0 (v3.1.0-rc1-207-g3cc4afdb71-dirty))
CFLAGS += -DDISABLE_PMP=1
endif

target_elf = $(filter %.elf,$(target))

.PHONY: run
run:
	qemu-system-riscv32 -nographic -machine virt -kernel $(target_elf)

# `run-gdb` and
# `riscv32-unknown-elf-gdb -ex "target remote localhost:1234"` from another terminal.
.PHONY: run-gdb
run-gdb:
	qemu-system-riscv32 -nographic -machine virt -kernel $(target_elf) -s -S

.PHONY: stop
stop:
	killall qemu-system-riscv32
