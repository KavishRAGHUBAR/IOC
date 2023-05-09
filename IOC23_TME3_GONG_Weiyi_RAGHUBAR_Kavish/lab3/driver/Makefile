CARD_NUMB       = 20 #numero 20 rpi
ROUTER          = peri
LOGIN           = raghubar_gong
LAB             = lab3

MODULE          = lcd_driver_RG

CROSSDIR        = /users/enseig/franck/IOC
KERNELDIR       = /dsk/l1/misc/linux-rpi-3.18.y
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-

all : clean default test.x upload
    
obj-m           += $(MODULE).o

default:;       make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

clean:;         make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean


%.x : %.c
	$(CROSS_COMPILE)gcc -o $@ $< -I$(INC) -O2 -static


upload:
	scp -P622$(CARD_NUMB) *.ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) *.sh pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) *.x pi@$(ROUTER):$(LOGIN)/$(LAB)
