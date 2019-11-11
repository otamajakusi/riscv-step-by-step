LOADER_LD = -L$(MAKEFILE_DIR) -Tloader.ld \
			-Wl,--nmagic -nostartfiles -nostdlib -nostdinc -static
ifneq ($(shell uname -r | awk /Microsoft$$/), "")
JLINK = JLink.exe
else
JLINK = JLinkExe
endif

loader = m.bin m.bin.o loader.o loader loader.hex

loader.o: $(MAKEFILE_DIR)/../steps/loader/loader.S
	$(CC) $(CFLAGS) -c -o $@ $<

m.bin.o: m.bin
	$(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $< $@ \
		--redefine-sym _binary_m_bin_start=_binary_start \
		--redefine-sym _binary_m_bin_end=_binary_end \
		--redefine-sym _binary_m_bin_size=_binary_size

loader: loader.o m.bin.o
	$(CC) $^ -o $@ -L$(MAKEFILE_DIR) -Tloader.ld -Wl,--nmagic -nostartfiles -nostdlib -nostdinc -static

loader.hex: loader
	$(OBJCOPY) -O ihex $< $@

%.bin : %.elf
	$(OBJCOPY) -O binary $< $@

# NOTE: currently only HiFive1-RevB is supported
.PHONY: upload
upload:
	printf "loadfile loader.hex\nrnh\nexit\n" | \
		$(JLINK) -device FE310 -if JTAG -speed 4000 -jtagconf -1,-1 -autoconnect 1
