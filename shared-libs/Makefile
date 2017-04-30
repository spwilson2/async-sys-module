CC:=gcc
CFLAGS += -std=gnu99 -nostdlib

TARGETS := libcircle-buffer.a

.PHONY: all clean
all: $(TARGETS)

libcircle-buffer.a: asm-primitives.o circle-buffer.o mutex.o
	ar rcs $@ $^
	rm $^

lib%.a: %.o
	ar rcs $@ $<

.PHONY: test
test:
	$(MAKE) -C $(PWD)/test test

clean: 
	rm -f $(TARGETS)
	$(MAKE) -C $(PWD)/test clean
