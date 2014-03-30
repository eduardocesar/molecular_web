#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <search.h>
#include <time.h>

#include "main_console.h"
#include "simulador.h"
#include "atomo_molecula.h"
#include "lennard_jones.h"
#include "ga.h"

int line_counter(char *s)
{
     int count = 0;
     char ch;
     unsigned idx = 0;
     do
     {
	  ch = s[idx++];
	  
	  if (ch == '\n') ++count;
     } while(ch != '\0');

     return count;
}

void print_help()
{
     printf(
	  "Command line parameters:\n"
	  "1 - Input file\n"
	  "2 - Output file (not implemented yet)\n"
	  "3 - Stats file\n"
	  "4 - Size of population\n"
	  "5 - Generations\n"
	  "6 - Prob crossover\n"
	  "7 - Prob mutation\n"
	  );
}

/**
 * @param[in, out] string to parse
 * @param[out] string with the unprocessed potentials
 * @param[out] string with the unprocessed molecule
 * @param[out] size of population
 * @param[out] generations
 * @param[out] crossover probability
 * @param[out] mutation probability
 */
static void process_param_string(char *params, char **string_potential, char **string_molecula, int *tam_populacao, int *geracoes, int *prob_crossover, int *prob_mutacao)
{
     char *separator, *tmp_sep;


     /* Finds the first chunk with the potential text */
     separator = strchr(params, 1);

     /* End of string */
     *separator = '\0';

     tmp_sep = separator+1;

     /* Copy the potential string */
     *string_potential = strdup(params);

     /* Molecule text */
     separator = strchr(tmp_sep, 1);
     *separator = '\0';

     /* Copy the molecule string */
     *string_molecula = strdup(tmp_sep);

     /* Population size number string */
     tmp_sep = separator+1;
     separator = strchr(tmp_sep, 1);
     *separator = '\0';

     /* Gets the population size */
     *tam_populacao = atoi(tmp_sep);

     /* Generations number string */
     tmp_sep = separator+1;
     separator = strchr(tmp_sep, 1);
     *separator = '\0';

     /* Gets the generations */
     *geracoes = atoi(tmp_sep);

     /* Crossover prob number string */
     tmp_sep = separator+1;
     separator = strchr(tmp_sep, 1);
     *separator = '\0';

     /* Gets the crossover prob */
     *prob_crossover = atoi(tmp_sep);

     /* Mutation probability number string */
     tmp_sep = separator+1;
     separator = strchr(tmp_sep, 1);
     *separator = '\0';

     /* Mutation probability */
     *prob_mutacao = atoi(tmp_sep);

}


void process_string_molecule(char *string_molecula, Molecule **molecula_entrada)
{


     char *buffer = alloca(1024);
     char *separator, *tmp_sep;

     int atom_counter = 0;
     char *elem = alloca(10);
     double x, y, z;

     int i, idx;

     int qtos_atomos = line_counter(string_molecula);

     Molecule *return_molecule;

     return_molecule = calloc(1, sizeof(Molecule));

     return_molecule->molecule = (Atom **) calloc(qtos_atomos, sizeof(Atom **));
     return_molecule->num_atoms = qtos_atomos;
     
     tmp_sep = string_molecula;

     for (i = 0; i < qtos_atomos; ++i)
     {
	  separator = strchr(tmp_sep, '\n');
	  idx = separator - tmp_sep;
	  memcpy(buffer, tmp_sep, idx);
	  memset(buffer+idx, 0, 1);
	  sscanf(buffer, "%s %lf %lf %lf", elem, &x, &y, &z);
	  return_molecule->molecule[atom_counter++] = create_atom(elem, x, y, z);
	  
	  tmp_sep += idx+1;
     }
     *molecula_entrada = return_molecule;
}

int newmain(char *params)
{
     int tam_populacao;
     int geracoes;
     int prob_crossover;
     int prob_mutacao;

     char *string_potential = NULL;
     char *string_molecula = NULL;

     Molecule *molecula_entrada = NULL;
     Molecule *molecula_otimizada = NULL;


     /* Breaks the \1 parameter string into the string fields */
     process_param_string(params, &string_potential, &string_molecula, &tam_populacao, &geracoes, &prob_crossover, &prob_mutacao);
     free(params);

     /* Estimate a number for the entries in the hash table that will be
	used for the potential comparison. */
     hcreate((int) line_counter(string_molecula)*1.5);

     /* Init the random number gen*/
     srand((unsigned int) time(NULL));



     /* printf("Tamanho da populacao: %s\n", argv[4]); */
     /* printf("Geracoes: %s\n", argv[5]); */
     /* printf("Prob crossover: %s\n", argv[6]); */
     /* printf("Prob mutacao: %s\n", argv[7]); */

     unsigned short POTENCIAL = LJ;

     /* Inicializa vetores de estatisticas globais */
     melhor_global = calloc(geracoes, sizeof(double));
     media_global = calloc(geracoes, sizeof(double));
     pior_global = calloc(geracoes, sizeof(double));

     
     switch (POTENCIAL)
     {
     case LJ:
	  
	  /* Process the params for the calculated potential
	     and constructs the hash table.*/
	  process_string_potential(string_potential);
	  free(string_potential);

	  /* Sets the potential function */
	  potencial = &potencial_lennard_jones;

	  /* Sets the local optimizer */
	  otimizador = &otimizar_lennard_jones;

	  /* Sets the energy calculator */
	  calcula_energia = get_energia_molecula_lennard_jones;
	  break;
     }


     //molecula_entrada = le_molecula_entrada(argv[1]);

     /* Process the string characters and return a molecule */
     process_string_molecule(string_molecula, &molecula_entrada);
     free(string_molecula);
     
     molecula_otimizada = otimizar_ga(molecula_entrada, geracoes, tam_populacao, prob_crossover, prob_mutacao);

     /* FILE *stats = fopen(argv[3], "w"); */
     int i;
     /* fprintf(stats, "Best, Average, Worst\n"); */
     /* for (i=0; i< geracoes; ++i) */
     /* { */
     /* 	  fprintf(stats, "%f %f %f\n", melhor_global[i], media_global[i], pior_global[i]); */
     /* } */
     
     destroy_molecule(molecula_entrada);
     /* destroy_molecule(molecula_teste); */

     hdestroy();
}
