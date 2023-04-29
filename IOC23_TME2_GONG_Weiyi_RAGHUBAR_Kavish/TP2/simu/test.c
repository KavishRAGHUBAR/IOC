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

// int main(void)
// {
    
//     char bp;

//     int file = open("/dev/led_RG", O_RDWR);
//     int file1 = open("/dev/led1_RG", O_RDWR);
//     int file2 = open("/dev/bp0_RG", O_RDWR);
//     if(file < 0 || file1 < 0 || file2 < 0) {
//         perror("open");
//         exit(errno);
//     }

//     while(1)
//     {
//         read(file2,&bp,1);
//        if(bp==0){write(file,'0',1);write(file1,'0',1);} 
//        else if(bp==1){write(file,'1',1);write(file1,'1',1);}
//     }


//     close(file);
//     close(file1);
//     close(file2);
//     return 0;
// }
int main()
{
   char led, bp='0';
   int fdled0 = open("/dev/led_RG", O_WRONLY);//Ouvrir les fichier de driver led en write only
   int fdbp = open("/dev/bp0_RG", O_RDONLY);//Ouvrir les fichier de driver bp en read only
   if ((fdled0 < 0)||(fdbp < 0)) {//Si ca a pas bien ouvert, donc on quit
      fprintf(stderr, "Erreur d'ouverture des pilotes LED ou Boutons\n");
      exit(1);
   }
   do { 
      led = (led == '0') ? '1' : '0';//Si led est 0, led change a 1, sinon, ca change a 0
        printf("led=%c",led);
      write( fdled0, &led, 1);//Write les valeur dans led

      sleep( 1);//wait 1 sec

      read( fdbp, &bp, 1);//lire les valeurs de bouton
      printf("bp=%c\n",bp);
   } while (bp == '1');//arrete le programme jusqu'a le bouton a pousse

//    while(1)
//    {
//     if(bp == 0)
//     {
//       led = (led == '0') ? '1' : '0';
//       printf("1");
//       write( fdled0, &led, 1);
//       printf("2");
//       sleep( 1);
//       printf("3");
//       read( fdbp, &bp, 1);
//       printf("bp=%c",bp);
//     }
//    }

    close(fdled0);
    close(fdbp);//fermer les fichiers
   return 0;
}