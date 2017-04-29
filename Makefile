all: main module

main:
	$(MAKE) -C lib-src

module:
	$(MAKE) -C module-src

clean:
	cd module-src && $(MAKE) clean
	$(MAKE) -C lib-src clean

load-module:
	sudo insmod module-src/as-sys.ko

unload-module:
	sudo rmmod as-sys
