CONFIG_MODULE_SIG=n

ifneq ($(KERNELRELEASE),)

include Kbuild

else
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
endif
