KDIR	     := /lib/modules/$(shell uname -r)/build
PWD	     := $(shell pwd)

ifneq ($(KERNELRELEASE),)

obj-m   := rosta_drv.o 

else

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	chmod 777 rosta_drv.ko

clean:
	rm -f .*.cmd rosta_drv.ko *.c~ *.h~ *.o Makefile~ 
	rm -rf .tmp_version
	rm -rf .tmp_versions
endif


