#include <stdlib.h>
#include <stdio.h>

int line_counter(char *s)
{
     FILE *fp = fopen(s, "r");
     int ch;
     int count=0;
     if (fp)
     {
	  do
	  {
	       ch = fgetc(fp);
	       if( ch== '\n') ++count;   
	  } while(ch != EOF);
     }
     else
     {
	  puts("File not found!");
	  exit(1);
     }
     fclose(fp);
     return count;
}


