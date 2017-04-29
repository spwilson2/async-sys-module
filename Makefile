KDIR ?= /lib/modules/`uname -r`/build

.PHONY:all
all: main module

.PHONY:main
main:
	$(MAKE) -C lib-src

.PHONY:module
module:
	$(MAKE) -C $(KDIR) M=$(PWD)/module-src

.PHONY:clean
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/module-src clean
	$(MAKE) -C lib-src clean

.PHONY:load-module
load-module:
	sudo insmod module-src/as-sys.ko

.PHONY:unload-module
unload-module:
	sudo rmmod as-sys
