LDFLAGS += -L$(MAKEFILE_DIR)../riscv-probe/env/virt -Tdefault.lds \

.PHONY: run
run:
	qemu-system-riscv32 -nographic -machine virt -kernel $(target)

# `run-gdb` and
# `riscv32-unknown-elf-gdb -ex "target remote localhost:1234"` from another terminal.
run-gdb:
	qemu-system-riscv32 -nographic -machine virt -kernel $(target) -s -S

.PHONY: stop
stop:
	killall qemu-system-riscv32
