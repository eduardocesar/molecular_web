#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "simulador.h"

int file_line_counter(char *s)
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
	  puts("Input file not found!");
	  exit(1);
     }
     fclose(fp);
     return count;
}

int file_size(char *s)
{
     FILE *fp = fopen(s, "r");
     fseek(fp, 0L, SEEK_END);
     long pos = ftell(fp);
     fclose(fp);
     return pos;
}     

/** 
 * Reads the necessary files and the command line options and constructs
 * the parameter string to be passed to the newmain function. 
 * 1 - parameter file 
 * 2 - molecule file
 * 3 - generations
 * 4 - crossover prob
 * 5 - mutation prob
*/
static char* build_param_string(char **argv)
{
     /* begin Refazer de modo mais eficiente */
     FILE *fp;
     char buf[2048];
     int sz1, sz2, sz3, sz4, sz5, sz6, sz7;


     sz1 = file_size(argv[1]);
     sz2 = file_size(argv[2]);

     sz3 = strlen(argv[3]);
     sz4 = strlen(argv[4]);
     sz5 = strlen(argv[5]);
     sz6 = strlen(argv[6]);
     sz7 = strlen(argv[7]);

     char *buffer = malloc(sz1+sz2+sz3+sz4+sz5+sz6+sz7+10);

     /* Le o arquivo no buffer */
     fp = fopen(argv[1], "r");
     fread(buf, sizeof(char), sz1, fp);
     fclose(fp);

     /* Copia o buffer no outro buffer */
     memcpy(buffer, buf, sz1);
     buffer[sz1] = '\1';


     /* Le o arquivo no buffer */
     fp = fopen(argv[2], "r");
     fread(buf, sizeof(char), sz2, fp);
     fclose(fp);

     /* Copia o buffer no outro buffer */
     memcpy(buffer+sz1+1, buf, sz2);
     buffer[sz1+sz2+1] = '\1';

     /* Copia o buffer no outro buffer */
     memcpy(buffer+sz1+sz2+2, argv[3], sz3);
     buffer[sz1+sz2+sz3+2] = '\1';

     /* Copia o buffer no outro buffer */
     memcpy(buffer+sz1+sz2+sz3+3, argv[4], sz4);
     buffer[sz1+sz2+sz3+sz4+3] = '\1';

     /* Copia o buffer no outro buffer */
     memcpy(buffer+sz1+sz2+sz3+sz4+4, argv[5], sz5);
     buffer[sz1+sz2+sz3+sz4+sz5+4] = '\1';

     /* Copia o buffer no outro buffer */
     memcpy(buffer+sz1+sz2+sz3+sz4+sz5+5, argv[6], sz6);
     buffer[sz1+sz2+sz3+sz4+sz5+sz6+5] = '\1';

     /* Copia o buffer no outro buffer */
     memcpy(buffer+sz1+sz2+sz3+sz4+sz5+sz6+6, argv[7], sz7);
     buffer[sz1+sz2+sz3+sz4+sz5+sz6+sz7+6] = '\1';

     /* end Refazer de modo mais eficiente */     
     return buffer;
}

/* TODO: Refazer */
static void process_result_string(char *params,  char **string_molecula)
{
     char *separator;

     /* Finds the first chunk with the potential text */
     separator = strchr(params, 1);

     /* End of string */
     *separator = '\0';

     /* tmp_sep = separator+1; */

     /* Copy the molecule string */
     *string_molecula = strdup(params);


}


int main(int argc, char **argv)
{
     char *params = build_param_string(argv);
     char *result = NULL;

     int i, geracoes = atoi(argv[3]);


     double energy;

     char *string_molecula = NULL;

     newmain(params, &result, &energy);
     printf("Energia = %f\n", energy);

     if (argc >= 9)
     {
	  process_result_string(result, &string_molecula);
	  FILE *fout = fopen(argv[8], "w");
	  fprintf(fout, "%s", string_molecula);
	  fclose(fout);

	  if (argc >= 10)
	  {
	       FILE *stats = fopen(argv[9], "w");
	       fprintf(stats, "melhor, media, pior\n");
	       for (i=0; i<geracoes; ++i)
	       {
		    fprintf(stats, "%f %f %f\n", melhor_global[i], media_global[i], pior_global[i]);
	       }
	       fclose(stats);
	  }
     }


     if (result) free(result);

     free(melhor_global);
     free(media_global);
     free(pior_global);

     free(params);
     return 0;
}
