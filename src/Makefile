subdirs = m
.PHONY: all $(subdirs)

all: $(subdirs)
clean: $(subdirs)

$(subdirs):
	$(MAKE) -C $@ CC=$(CC) $(MAKECMDGOALS)
