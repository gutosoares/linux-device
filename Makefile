obj-m += device.o
device-y = device_main.o
device-y+= video1.o
device-y+= video2.o
device-y+= video3.o
device-y+= video4.o
device-y+= video5.o

KERNELDIR	?=  /lib/modules/$(shell uname -r)/build
PWD		:= $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
