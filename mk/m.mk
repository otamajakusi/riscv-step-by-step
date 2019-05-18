LDFLAGS += -L$(MAKEFILE_DIR)../riscv-probe/env/qemu-sifive_u -Tdefault.lds \

.PHONY: run
run:
	qemu-system-riscv32 -nographic -machine sifive_u -kernel $(target)

.PHONY: stop
stop:
	killall qemu-system-riscv32
