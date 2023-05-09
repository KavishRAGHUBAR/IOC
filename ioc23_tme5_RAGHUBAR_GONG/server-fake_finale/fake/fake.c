#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <time.h>


#define MAXServerResquest 1024

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

static int gpio_read (uint32_t pin)
{
    uint32_t reg = pin / 32;
    uint32_t bit = pin % 32;
    uint32_t mask = 1;
    return gpio_regs_virt->gplev[reg]>>bit & mask !=0;
}

void
delay ( unsigned int milisec )
{
    struct timespec ts, dummy;
    ts.tv_sec  = ( time_t ) milisec / 1000;
    ts.tv_nsec = ( long ) ( milisec % 1000 ) * 1000000;
    nanosleep ( &ts, &dummy );
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
int BP_ON = 0, BP_OFF = 0;
int
main ( int argc, char **argv )
{
    int     f2s, s2f;
    char    *f2sName = "/tmp/f2s_fw";
    char    *s2fName = "/tmp/s2f_fw";
    char    serverRequest[MAXServerResquest];
    char    serverBouton[]=" ";
    fd_set  rfds;
    struct  timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    mkfifo(s2fName, 0666); 
    mkfifo(f2sName, 0666);

    s2f = open(s2fName, O_RDWR);
    f2s = open(f2sName, O_RDWR);



    // map GPIO registers
    // ---------------------------------------------

    if ( gpio_mmap ( (void **)&gpio_regs_virt ) < 0 ) {
        printf ( "-- error: cannot setup mapped GPIO.\n" );
        exit ( 1 );
    }

    // Setup GPIO of LED0 to output
    // ---------------------------------------------
    
    gpio_fsel(GPIO_LED0, GPIO_FSEL_OUTPUT);
    gpio_fsel(GPIO_BP, GPIO_FSEL_INPUT);

    int val_prec = 1;
    int val_nouv = 1;

    do {
        FD_ZERO(&rfds);
        FD_SET(s2f, &rfds);

        if (select(s2f+1, &rfds, NULL, NULL, &tv) != 0) {
            if (FD_ISSET(s2f, &rfds)) {
                int nbchar;
                int val = 0;
                if ((nbchar = read(s2f, serverRequest, MAXServerResquest)) == 0) break;
                serverRequest[nbchar]=0;
                fprintf(stderr,"%s", serverRequest);
                
                // La chaîne de caractère commence par "w "
                // On lit donc la troisième
                if(serverRequest[2] == '0'){
                    val = 0;
                }
                else if(serverRequest[2] == '1'){
                    val = 1;
                }
                //écriture dans LED0
                gpio_write ( GPIO_LED0, val );

                write(f2s, serverRequest, nbchar);
            }
        }

        val_nouv = gpio_read(GPIO_BP);
        if (val_prec != val_nouv) {
            if (val_nouv == 0) {
                BP_ON = 1;
                serverBouton[2]='1';
                //printf("bp is on\n");
                //fprintf(stderr,"%s", serverBouton);
                write(f2s, serverBouton,1);
            }
            else{
                BP_OFF = 1;
                serverBouton[2]='0';
                //printf("bp is off\n");
                //fprintf(stderr,"%s", serverBouton);
                write(f2s, serverBouton,1);
            }
            val_prec = val_nouv;
        } 


        if (BP_ON){
            BP_ON = 0;
            
        }
        if (BP_OFF){
            BP_OFF = 0;
            
        } 
        
    }
    while (1);

    close(f2s);
    close(s2f);

    return 0;
}
