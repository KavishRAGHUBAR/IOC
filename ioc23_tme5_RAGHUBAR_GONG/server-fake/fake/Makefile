APPS = led_bp
CROSSDIR = /users/enseig/franck/IOC
CFLAGS=-Wall -Werror
CROSS_COMPILE = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-
LDFLAGS = -static

all 	: $(APPS)  
clean 	:;rm $(APPS) *~ f2s s2f 2> /dev/null || true
%.x: %.c
	$(CROSS_COMPILE)gcc -o $@ $^ $(LDFLAGS) -lpthread
