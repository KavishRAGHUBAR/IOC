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
#include <linux/ioctl.h>
struct cord_xy {
int line;
int row;
} cord_xy;
#define IOC_MAGIC 't'
#define LCDIOCT_CLEAR _IO(IOC_MAGIC, 20)
#define LCDIOCT_SETXY _IOW(IOC_MAGIC, 21, struct cord_xy)

//------------------------------------------------------------------------------
// GPIO ACCES
//------------------------------------------------------------------------------


int main()
{
   char lcd='0';

   //char *txt="He\nll\bo";
   //int taille=strlen(txt);
   //printf("%d",taille);//Essayer a lire le longeur ici
   int fdlcd = open("/dev/lcd_driver_RG", O_WRONLY);

   if ((fdlcd < 0)) {
      fprintf(stderr, "Erreur d'ouverture des pilotes lcds\n");
      exit(1);
   }
   write( fdlcd, "He\bllo\nRAGHUBAR\nGONG",1);//On  a inserer les chaines comme sa
   //ioctl(fdlcd, LCDIOCT_CLEAR);

   close(fdlcd);
   return 0;
}