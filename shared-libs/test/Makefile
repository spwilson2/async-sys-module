CFLAGS:=-ggdb -O2
CC:=gcc
LDFLAGS:=-pthread
circle-buffer-test: circle-buffer-test.o ../libcircle-buffer.a

mutex-test: mutex-test.o

test-mutex: mutex-test
	`pwd`/mutex-test

test-circle-buffer: circle-buffer-test
	`pwd`/circle-buffer-test

test: test-mutex test-circle-buffer

clean:
	rm -f circle-buffer-test mutex-test
