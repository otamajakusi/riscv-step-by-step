LDFLAGS += -L$(MAKEFILE_DIR)../riscv-probe/env/qemu-sifive_u -Tdefault.lds \


.PHONY: test
test:
	qemu-system-riscv32 -nographic -machine sifive_u -kernel $(target)
