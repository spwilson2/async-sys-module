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

clean: 
	rm -f $(TARGETS)
