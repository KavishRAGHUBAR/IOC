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


int main()
{
   char lcd='0';
   int fdlcd = open("/dev/lcd0_RG", O_WRONLY);

   if ((fdlcd < 0)) {
      fprintf(stderr, "Erreur d'ouverture des pilotes lcds\n");
      exit(1);
   }
   write( fdlcd, "Hello", 1);

    close(fdlcd);
   return 0;
}