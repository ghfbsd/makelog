/*
   reformat -- Program to fix formatting problems with A-type records in a
   SEED volume.  Orfeus makes SEED volumes that have the first block of a
   continued set of A blocks with a '*' flag.  The second one should rather
   be flagged.
*/

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[]){
   char buf[512];
   int flag = 0;
   do {
      ssize_t n = read(0, buf, sizeof(buf));

      if (n<=0) break;

      if (n != sizeof(buf)) {
         fprintf(stderr, "%s: Unexpected buffer size %d\n",argv[0],(int)n);
	 exit(1);
      }

      if (0 == strncmp(buf,"000002A*",8)) buf[7] = ' ', flag = 1;

      if (flag && 0 == strncmp(buf,"000003A ",8)) buf[7] = '*';

      if (n != write(1, buf, sizeof(buf))) exit(1);
   } while(1);
   return 0;
}
