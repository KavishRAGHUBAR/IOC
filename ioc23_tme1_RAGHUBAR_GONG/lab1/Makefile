NAME = raghubar_gong # nom1_nom2
RPI ?= 20 # card_number

CROSS_COMPILE ?= bcm2708hardfp-

CFLAGS=-Wall -Wfatal-errors -O2
LDFLAGS=-static  

help:
	@echo ""
	@echo "make [RPI=num] target"
	@echo ""
	@echo "      target :	help	this message"
	@echo " 		all	clean all and recompile"
	@echo " 		dep	compile if required"
	@echo " 		clean	clean all except sources"
	@echo " 		copy	compile then copy to card $(RPI) in directory $(NAME)"
	@echo ""

all: clean dep 

dep : blink1_bp_pt.x
#blink0.x blink0_pt.x blink1_pt.x read_bp.x 
copy: *.x
	scp -P 622$(RPI) $^ pi@peri:$(NAME)

%.x: %.c
	$(CROSS_COMPILE)gcc -o $@ $^ $(LDFLAGS) -lpthread -D_REENTRANT

clean:
	rm -f *.o *.x *~ *.a
