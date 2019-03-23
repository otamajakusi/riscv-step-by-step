LDFLAGS += -L$(MAKEFILE_DIR)../riscv-probe/env/qemu-sifive_u -Tdefault.lds \
           $(MAKEFILE_DIR)../riscv-probe/build/obj/rv32imac/env/qemu-sifive_u/crt.o \
           $(MAKEFILE_DIR)../riscv-probe/build/obj/rv32imac/env/qemu-sifive_u/setup.o

.PHONY: test
test:
	qemu-system-riscv32 -nographic -machine sifive_u -kernel $(target)
