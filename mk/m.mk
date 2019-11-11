LDFLAGS += -L$(MAKEFILE_DIR)../riscv-probe/env/virt -Tdefault.lds
QEMU_VERSION = $(shell qemu-system-riscv32 -version | awk /^QEMU/)
ifneq ($(QEMU_VERSION), QEMU emulator version 3.1.0 (v3.1.0-rc1-207-g3cc4afdb71-dirty))
CFLAGS += -DDISABLE_PMP=1
endif

ifneq ($(shell uname -r | awk /Microsoft$$/), "")
JLINK = JLink.exe
else
JLINK = JLinkExe
endif

%.hex : %.elf
	$(OBJCOPY) -O ihex $< $@

target_elf = $(filter %.elf,$(target))
target_hex = $(filter %.hex,$(target))

.PHONY: run
run:
	qemu-system-riscv32 -nographic -machine virt -kernel $(target_elf)

# `run-gdb` and
# `riscv32-unknown-elf-gdb -ex "target remote localhost:1234"` from another terminal.
.PHONY: run-gdb
run-gdb:
	qemu-system-riscv32 -nographic -machine virt -kernel $(target_elf) -s -S

# NOTE: currently only HiFive1-RevB is supported
.PHONY: upload
upload:
	echo -e "loadfile $(target_hex)\nrnh\nexit" | \
		$(JLINK) -device FE310 -if JTAG -speed 4000 -jtagconf -1,-1 -autoconnect 1

.PHONY: stop
stop:
	killall qemu-system-riscv32
