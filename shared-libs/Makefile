CC:=gcc
CFLAGS += -std=gnu99 -ggdb -O2

TARGETS := libcircle_buffer.a

.PHONY: all clean
all: $(TARGETS)

libcircle_buffer.a: circle_buffer.o
	ar rcs $@ $^
	rm $^

.PHONY: test
test: libcircle_buffer.a
	$(MAKE) -C $(PWD)/test test

clean: 
	rm -f $(TARGETS)
	$(MAKE) -C $(PWD)/test clean
