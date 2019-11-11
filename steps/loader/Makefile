srcs  = loader.S
target = loader.elf loader.hex
objs = m.bin.o
include ../../mk/common.mk
include ../../mk/m.mk

../1/m.bin:
	$(MAKE) -C ../1

m.bin.o: ../1/m.bin
	$(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $< $@ \
		--redefine-sym _binary____1_m_bin_start=_binary_start \
		--redefine-sym _binary____1_m_bin_end=_binary_end \
		--redefine-sym _binary____1_m_bin_size=_binary_size

loader.elf: loader.o m.bin.o
	$(CC) $^ -o $@ -L. -Tloader.ld -Wl,--nmagic -nostartfiles -static
