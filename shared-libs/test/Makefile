CC:=gcc
circle-buffer-test: LDFLAGS=-pthread
circle-buffer-test: circle-buffer-test.o ../libcircle-buffer.a

test: circle-buffer-test
	`pwd`/circle-buffer-test

clean:
	rm -f circle-buffer-test
