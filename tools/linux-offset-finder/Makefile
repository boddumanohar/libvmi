obj-m +=findoffsets.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules -lcsys

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
