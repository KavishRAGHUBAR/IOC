#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>


#define MAXServerResquest 1024

#define BCM2835_PERIPH_BASE     0x20000000
#define BCM2835_GPIO_BASE       ( BCM2835_PERIPH_BASE + 0x200000 )

#define GPIO_LED0   4
#define GPIO_LED1   17
#define GPIO_BP     18

#define GPIO_FSEL_INPUT  0
#define GPIO_FSEL_OUTPUT 1

struct gpio_s
{
    uint32_t gpfsel[7];
    uint32_t gpset[3];
    uint32_t gpclr[3];
    uint32_t gplev[3];
    uint32_t gpeds[3];
    uint32_t gpren[3];
    uint32_t gpfen[3];
    uint32_t gphen[3];
    uint32_t gplen[3];
    uint32_t gparen[3];
    uint32_t gpafen[3];
    uint32_t gppud[1];
    uint32_t gppudclk[3];
    uint32_t test[1];
};

struct gpio_s *gpio_regs_virt; 


static void 
gpio_fsel(uint32_t pin, uint32_t fun)
{
    uint32_t reg = pin / 10;
    uint32_t bit = (pin % 10) * 3;
    uint32_t mask = 0b111 << bit;
    gpio_regs_virt->gpfsel[reg] = (gpio_regs_virt->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

static void 
gpio_write (uint32_t pin, uint32_t val)
{
    uint32_t reg = pin / 32;
    uint32_t bit = pin % 32;
    if (val == 1) 
        gpio_regs_virt->gpset[reg] = (1 << bit);
    else
        gpio_regs_virt->gpclr[reg] = (1 << bit);
}



#define RPI_PAGE_SIZE           4096
#define RPI_BLOCK_SIZE          4096

static int mmap_fd;

static int
gpio_mmap ( void ** ptr )
{
    void * mmap_result;

    mmap_fd = open ( "/dev/mem", O_RDWR | O_SYNC );

    if ( mmap_fd < 0 ) {
        return -1;
    }

    mmap_result = mmap (
        NULL
      , RPI_BLOCK_SIZE
      , PROT_READ | PROT_WRITE
      , MAP_SHARED
      , mmap_fd
      , BCM2835_GPIO_BASE );

    if ( mmap_result == MAP_FAILED ) {
        close ( mmap_fd );
        return -1;
    }

    *ptr = mmap_result;

    return 0;
}

void
gpio_munmap ( void * ptr )
{
    munmap ( ptr, RPI_BLOCK_SIZE );
}


int main()
{
    int     f2s, s2f;                                       // fifo file descriptors
    char    *f2sName = "/tmp/f2s_rg";                       // filo names
    char    *s2fName = "/tmp/s2f_rg";                       //
    char    serverRequest[MAXServerResquest];               // buffer for the request
    fd_set  rfds;                                           // flag for select
    struct  timeval tv;                                     // timeout
    tv.tv_sec = 1;                                          // 1 second
    tv.tv_usec = 0;                                         //
	
    int led_state;
    fprintf(stderr,"let's start\n");
    mkfifo(s2fName, 0666);                                  // fifo creation
    mkfifo(f2sName, 0666);

    /* open both fifos */
    s2f = open(s2fName, O_RDWR);                            // fifo openning
    f2s = open(f2sName, O_RDWR);
    
    if ( gpio_mmap ( (void **)&gpio_regs_virt ) < 0 ) {
        printf ( "-- error: cannot setup mapped GPIO.\n" );
        exit ( 1 );
    }

    gpio_fsel(GPIO_LED1,GPIO_FSEL_OUTPUT);
    fprintf(stderr,"let's start\n");

    do {
        int nbchar;
        while ((nbchar = read(s2f, serverRequest, MAXServerResquest)) == 0);
        fprintf(stderr,"1");
        serverRequest[nbchar]=0;
        fprintf(stderr,"2");
        fprintf(stderr,"%s%s", serverRequest, serverRequest);
        fprintf(stderr,"3");
        write(f2s, serverRequest, nbchar);
        fprintf(stderr,"4");
    	if(serverRequest[2] == '0') {
	        led_state = 0;
	    }  
		if(serverRequest[2] == '1') {
	        led_state = 1;
        }
        fprintf(stderr,"5");
		gpio_write(GPIO_LED1,led_state);
        fprintf(stderr,"6");
/*
        FD_ZERO(&rfds);                                     
        FD_SET(s2f, &rfds);                                 
        if (select(s2f+1, &rfds, NULL, NULL, &tv) != 0) {   
            if (FD_ISSET(s2f, &rfds)) {                     
                int nbchar;
                if ((nbchar = read(s2f, serverRequest, MAXServerResquest)) == 0) {fprintf(stderr,"BREAK\n");continue;}
                serverRequest[nbchar]=0;
                fprintf(stderr,"%s%s", serverRequest, serverRequest);
                write(f2s, serverRequest, nbchar);
	        	if(serverRequest[2] == '0') {
			        led_state = 0;
		        }  
		        if(serverRequest[2] == '1') {
			        led_state = 1;
		        }
		        gpio_write(GPIO_LED1,led_state);
            }
        }
*/
    }
    while (1);

    close(f2s);
    close(s2f);

    return 0;
}
