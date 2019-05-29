LDFLAGS += -L$(MAKEFILE_DIR)../riscv-probe/env/qemu-sifive_u -Tdefault.lds \

.PHONY: run
run:
	qemu-system-riscv32 -nographic -machine sifive_u -kernel $(target)

# `run-gdb` and
# `riscv32-unknown-elf-gdb -ex "target remote localhost:1234"` from another terminal.
run-gdb:
	qemu-system-riscv32 -nographic -machine sifive_u -kernel $(target) -s -S

.PHONY: stop
stop:
	killall qemu-system-riscv32
