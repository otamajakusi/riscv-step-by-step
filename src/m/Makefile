srcs  = crt.S
objs  = $(patsubst %S,%o,$(filter %.S,$(srcs)))
objs += $(patsubst %c,%o,$(filter %.c,$(srcs)))
target = m.elf

LDFLAGS = -Wl,--gc-sections -L../../riscv-probe/build/lib/rv32imac -nostartfiles -nostdlib -nostdinc -static

.PHONY: all
all: $(objs) $(target)

$(target): $(objs)
	$(CC) $(LDFLAGS) $(objs) -lfemto -o $@

%.o : %.S
	$(CC) -c -o $@ $<

%.o : %.c
	$(CC) -c -o $@ $<

clean:
	$(RM) $(objs)
