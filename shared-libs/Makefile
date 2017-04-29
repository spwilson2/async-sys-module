CC:=gcc
CFLAGS += -std=gnu99 -nostdlib

TARGETS := libcircle-buffer.a libmutex.a

.PHONY: all clean
all: $(TARGETS)

lib%.a: %.o
	ar rcs $@ $<

clean: 
	rm -f $(TARGETS)
