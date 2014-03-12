#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <search.h>
#include <time.h>

#include "misc_functions.h"
#include "simulador.h"
#include "atomo_molecula.h"
#include "lennard_jones.h"
#include "ga.h"

/* Reads the input file and returns the Molecule structure with the
 * contents */

Molecule *le_molecula_entrada(char *s)
{
     int size = 1024;

     int pos, c, atom_counter = 0;
     char *buffer = (char *) malloc(size);


     char *elem = alloca(10);
     double x, y, z;

     int qtos_atomos = line_counter(s);

     Molecule *return_molecule = calloc(1, sizeof(Molecule));

     return_molecule->molecule = (Atom **) calloc(qtos_atomos, sizeof(Atom **));
     return_molecule->num_atoms = qtos_atomos;

     FILE *fin = fopen(s, "r");

     if (fin)
     {
	  while (1)  /* read all lines in file */
	  {
	       pos = 0;
	       do /* read one line */
	       {
		    c = fgetc(fin);
			 
		    if (c != EOF) buffer[pos++] = (char) c;
		    if(pos >= size - 1) { // increase buffer length - leave room for 0
			 size *= 2;
			 buffer = (char*) realloc(buffer, size);
		    }
		    
	       } while(c != '\n' && c != EOF);

	       if (c == EOF) break;
	       buffer[pos] = 0;	/* End of line */
	       //printf("%s", buffer);

	       /* Constructs one atom and stores it in the molecule */
	       sscanf(buffer, "%s %lf %lf %lf", elem, &x, &y, &z);

	       return_molecule->molecule[atom_counter] = create_atom(elem, x, y, z);
	       ++atom_counter;
	       
	  }

     }
     else
     {
	  puts("Input file not found!");
	  exit(1);
     }

     free(buffer);
     fclose(fin);
     return return_molecule;
}



int main(int argc, char **argv)
{
     /* Verifica numero de parametros da linha de comando */
     if (argc !=8)
     {
	  puts("Error in the command line parameters!");
	  exit(1);
     }

     /* Init the random number gen*/
     srand((unsigned int) time(NULL));

     /* Estimate a number for the entries in the hash table. */
     
     hcreate((int) line_counter(ARQUIVO_POTENCIAL_LENNARD_JONES)*1.5);



     int tam_populacao = atoi(argv[4]);
     int geracoes = atoi(argv[5]);
     int prob_crossover = atoi(argv[6]);
     int prob_mutacao = atoi(argv[7]);

     printf("Tamanho da populacao: %s\n", argv[4]);
     printf("Geracoes: %s\n", argv[5]);
     printf("Prob crossover: %s\n", argv[6]);
     printf("Prob mutacao: %s\n", argv[7]);

     unsigned short POTENCIAL = LJ;

     /* Inicializa vetores de estatisticas globais */
     melhor_global = calloc(geracoes, sizeof(double));
     media_global = calloc(geracoes, sizeof(double));
     pior_global = calloc(geracoes, sizeof(double));

     
     /* Le o arquivo de potenciais, de acordo com o potencial escolhido */
     switch (POTENCIAL)
     {
     case LJ:
	  
	  /* Reads the file and constructs parameter hash table */
	  le_potencial_lennard_jones();

	  /* Sets the potential function */
	  potencial = &potencial_lennard_jones;

	  /* Sets the local optimizer */
	  otimizador = &otimizar_lennard_jones;

	  /* Sets the energy calculator */
	  calcula_energia = get_energia_molecula_lennard_jones;
	  break;
     }


     /* Le o arquivo de dados e constroi a molecula inicial */
     Molecule *molecula_entrada;
     Molecule *molecula_otimizada;

     molecula_entrada = le_molecula_entrada(argv[1]);
     
     molecula_otimizada = otimizar_ga(molecula_entrada, geracoes, tam_populacao, prob_crossover, prob_mutacao);

     FILE *stats = fopen(argv[3], "w");
     int i;
     fprintf(stats, "Best, Worst, Average");
     for (i=0; i< tam_populacao; ++i)
     {
	  fprintf(stats, "%f %f %f\n", melhor_global[i], pior_global[i], media_global[i]);
     }
     
     destroy_molecule(molecula_entrada);
     /* destroy_molecule(molecula_teste); */

     hdestroy();
}
