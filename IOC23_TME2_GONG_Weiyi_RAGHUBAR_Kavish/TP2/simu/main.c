//------------------------------------------------------------------------------
// Headers that are required for printf and mmap
//------------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

//------------------------------------------------------------------------------
// GPIO ACCES
//------------------------------------------------------------------------------

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





delay ( unsigned int milisec )
{
    struct timespec ts, dummy;
    ts.tv_sec  = ( time_t ) milisec / 1000;
    ts.tv_nsec = ( long ) ( milisec % 1000 ) * 1000000;
    nanosleep ( &ts, &dummy );
}

// int
// main ( int argc, char **argv )
// {
//     // Get args
//     // ---------------------------------------------

//     int period, half_period;

//     period = 1000; /* default = 1Hz */
//     if ( argc > 1 ) {
//         period = atoi ( argv[1] );
//     }
//     half_period = period / 2;
//     uint32_t volatile * gpio_base = 0;

//     // map GPIO registers
//     // ---------------------------------------------

//     if ( gpio_mmap ( (void **)&gpio_regs_virt ) < 0 ) {
//         printf ( "-- error: cannot setup mapped GPIO.\n" );
//         exit ( 1 );
//     }

//     // Setup GPIO of LED0 to output
//     // ---------------------------------------------
    
//     gpio_fsel(GPIO_LED0, GPIO_FSEL_OUTPUT);

//     gpio_fsel(GPIO_LED1, GPIO_FSEL_OUTPUT);

//     // Blink led at frequency of 1Hz
//     // ---------------------------------------------

//     uint32_t val = 0;

//     printf ( "-- info: start blinking.\n" );

//     while (1) {
//         gpio_write ( GPIO_LED0, val );
//         //delay ( half_period );
//         val = 1 - val;
//         gpio_write ( GPIO_LED1, val );
//         delay ( half_period );
//     }

//     return 0;
// }

int main(void)
{
    
    int val=0;
    int bp;
    int etat=0;

    int file = open("/dev/led_RG", O_RDWR);
    int file1 = open("/dev/led1_RG", O_RDWR);
    int file2 = open("/dev/bp0_RG", O_RDWR);
    if(file < 0) {
        perror("open");
        exit(errno);
    }


    int val_prec = 1;
    int val_nouv = 1;
    int BP_ON = 0;
    int BP_OFF = 0;
    while(1)
    {
        delay(20);
        val_nouv=read(file2,'1',1);
        //printf("val=%d",val_nouv);
        if(val_prec != val_nouv)
        {
            if(val_nouv == 0)
            {
                BP_ON = 1;
                
            }
            else if(val_nouv == 1)
            {
                BP_OFF = 1;
                
            }
            val_prec = val_nouv;
        }

        //printf("BP_ON=%d\n",BP_ON);
        //printf("BP_OFF=%d\n",BP_OFF);


    }

    while(1)
    {
        if (BP_ON == 1){
            BP_ON = 0;      
            write(file, 'val',1);
            write(file1,'val',1);
            

        }
        if (BP_OFF == 1){
            BP_OFF = 0;
            val = 1 - val;
        }
    }

    close(file);
    close(file1);
    close(file2);
    return 0;
}