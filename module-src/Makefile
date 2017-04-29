ifneq ($(KERNELRELEASE),)
	# kbuild part of makefile
obj-m := hello.o

else
	# normal makefile
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

# Module specific targets
genbin:
	echo "X" > hello.o_shipped
endif
