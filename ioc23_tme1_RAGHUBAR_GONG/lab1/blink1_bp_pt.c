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
#include <pthread.h>

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

int BP_ON = 0;
int BP_OFF = 0;

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

static int 
gpio_read(uint32_t pin)
{
    //int lev;
    uint32_t reg = pin / 32;
    uint32_t bit = pin % 32;
    uint32_t mask = 1;
    return gpio_regs_virt->gplev[reg]>>bit & mask !=0;
    //return gpio_regs_virt->gplev[reg] & mask;//lev=

}

//------------------------------------------------------------------------------
// Access to memory-mapped I/O
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
// Main Programm
//------------------------------------------------------------------------------

void
delay ( unsigned int milisec )
{
    struct timespec ts, dummy;
    ts.tv_sec  = ( time_t ) milisec / 1000;
    ts.tv_nsec = ( long ) ( milisec % 1000 ) * 1000000;
    nanosleep ( &ts, &dummy );
}

void* clig0(void* freq)
{
    unsigned freq_un = *((unsigned int*)freq);

    // Blink led at frequency of 1Hz
    // ---------------------------------------------

    uint32_t val = 0;


    while (1) {

        if (BP_ON == 1){
            BP_ON = 0;      
            gpio_write ( GPIO_LED0, val );

        }
        if (BP_OFF == 1){
            BP_OFF = 0;
            val = 1 - val;
        }
    

        
    }
    
    
}

void* clig1(void* freq)
{

    unsigned freq_un = *((unsigned int*)freq);
    // Blink led at frequency of 1Hz
    // ---------------------------------------------

    uint32_t val = 0;

    while (1) {
        gpio_write ( GPIO_LED1, val );
        delay ( freq_un );
        val = 1 - val;
    }
}

void* telerupteur(void* freq)
{

    unsigned freq_un = *((unsigned int*)freq);
    int val_prec = 1;
    int val_nouv = 1;
    while(1)
    {
        delay(freq_un);
        val_nouv = gpio_read(GPIO_BP);
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
    }
}

int
main ( int argc, char **argv )
{
    // Get args
    // ---------------------------------------------

    int period, half_period,period_bouton;

    period = 1000; /* default = 1Hz */
    if ( argc > 1 ) {
        period = atoi ( argv[1] );
    }
    half_period = period / 2;
    period_bouton = period / 50;
    uint32_t volatile * gpio_base = 0;

    // map GPIO registers
    // ---------------------------------------------

    if ( gpio_mmap ( (void **)&gpio_regs_virt ) < 0 ) {
        printf ( "-- error: cannot setup mapped GPIO.\n" );
        exit ( 1 );
    }

    // Setup GPIO of LED0 to output
    // ---------------------------------------------
    
    gpio_fsel(GPIO_LED0, GPIO_FSEL_OUTPUT);

    gpio_fsel(GPIO_LED1, GPIO_FSEL_OUTPUT);

    gpio_fsel(GPIO_BP, GPIO_FSEL_INPUT);

    // Blink led at frequency of 1Hz
    // ---------------------------------------------

    uint32_t val = 0;

    printf ( "-- info: start blinking.\n" );
   /*
    while (1) {
        val=gpio_read(GPIO_BP);
        if(val==0){
            
            
            printf("1\n");
            
        }else 
        {printf("2\n");
        }
        sleep(1);
    }*/
    /* while (1) {
        printf("va  l=%d",val);
        val = gpio_read(GPIO_BP);
        //gpio_write( GPIO_LED1, val );
        delay(half_period);
    }
    pid_t pid = fork();
    if(pid == 0)
    {
        while (1) {
            gpio_write ( GPIO_LED0, val );
            delay ( half_period );
            val = 1 - val;
        }
    }
    else
    {
        while (1) {
            gpio_write ( GPIO_LED1, val );
            delay ( period );
            val = 1 - val;
        }
    }
    */
     

    pthread_t t1,t2,t3;

    pthread_create(&t1,NULL,clig0,&half_period);
    pthread_create(&t2,NULL,clig1,&period);
    pthread_create(&t3,NULL,telerupteur,&period_bouton);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    
    
    while(1);
   

    return 0;
}
