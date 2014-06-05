#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <search.h>
#include <time.h>

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

/* Takes a vector os doubles and transform in a string */
static char* string_vec_double(double *vec, unsigned int size)
{
     char *result = NULL, *tmp = NULL;
     int i;
     
     asprintf(&result, "%f", vec[0]);

     for (i=1; i<size; ++i)
     {
	  asprintf(&tmp, "%s, %f", result, vec[i]);
	  free(result);
	  result = tmp;
     }
     return result;
}

/* Transforms a struct molecule in a string */
static void molecule_string(const Molecule *mol, char **string_mol)
{
     char *result = malloc(50);
     char *tmp = NULL;

     unsigned int num_atoms = mol->num_atoms;
     double x, y, z;
     char line[100];
     int i;
     
     //memset(result, '\0', 1);
     sprintf(result, "%d\nA structure with %d atoms.\n", num_atoms, num_atoms);

     for (i=0; i<num_atoms; ++i)
     {
	  x = mol->molecule[i]->x;
	  y = mol->molecule[i]->y;
	  z = mol->molecule[i]->z;
	  sprintf(line, "%s %f %f %f\n", mol->molecule[i]->element, x, y, z);

	  asprintf(&tmp, "%s%s", result, line);
	  free(result);
	  result = tmp;
     }
     *string_mol = result;
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
static void process_param_string(char *params, char **string_potential, char **string_molecula, int *geracoes, int *tam_populacao, int *prob_crossover, int *prob_mutacao, unsigned char *mode)
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
     *geracoes = atoi(tmp_sep);

     /* Generations number string */
     tmp_sep = separator+1;
     separator = strchr(tmp_sep, 1);
     *separator = '\0';

     /* Gets the generations */
     *tam_populacao = atoi(tmp_sep);

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


     /* Mode number string */
     tmp_sep = separator+1;
     separator = strchr(tmp_sep, 1);
     *separator = '\0';

     /* Mode */
     *mode = atoi(tmp_sep);
}


static void process_string_molecule(char *string_molecula, Molecule **molecula_entrada)
{
     char *buffer = alloca(1024);
     char *separator, *tmp_sep;

     int atom_counter = 0;
     char *elem = alloca(10);
     double x, y, z;

     int i, idx, counter = -1;

     int qtos_atomos = line_counter(string_molecula);

     Molecule *return_molecule;

     return_molecule = calloc(1, sizeof(Molecule));

     return_molecule->molecule = (Atom **) calloc(qtos_atomos-2, sizeof(Atom **));
     return_molecule->num_atoms = qtos_atomos-2;
     
     tmp_sep = string_molecula;

     for (i = 0; i < qtos_atomos; ++i)
     {
	  ++counter;
	  separator = strchr(tmp_sep, '\n');
	  idx = separator - tmp_sep;
	  if (counter >= 2)  /* Skips the 2 first lines */
	  {
	       memcpy(buffer, tmp_sep, idx);
	       memset(buffer+idx, 0, 1);
	       sscanf(buffer, "%s %lf %lf %lf", elem, &x, &y, &z);
	       return_molecule->molecule[atom_counter++] = create_atom(elem, x, y, z);
	  }
	  tmp_sep += idx+1;
     }
     *molecula_entrada = return_molecule;
}



void newmain(char *params, char **result, double *energy)
{
     int geracoes;
     int tam_populacao;

     int prob_crossover;
     int prob_mutacao;

     unsigned char mode = 0;

     char *string_potential = NULL;
     char *string_molecula = NULL;

     char *molecule = NULL;

     Molecule *molecula_entrada = NULL;
     Molecule *molecula_otimizada = NULL;

     /* Breaks the \1 parameter string into the string fields */
     process_param_string(params, &string_potential, &string_molecula, &geracoes, &tam_populacao, &prob_crossover, &prob_mutacao, &mode);

     /* Estimate a number for the entries in the hash table that will be
	used for the potential comparison. */
     hcreate((int) line_counter(string_molecula)*1.5);

     /* Init the random number gen*/
     srand((unsigned int) time(NULL));

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


     /* Process the string characters and return a molecule */
     process_string_molecule(string_molecula, &molecula_entrada);
     free(string_molecula);

     switch (mode)
     {
     case 0:
	  molecula_otimizada = otimizar_ga(molecula_entrada, geracoes, tam_populacao, prob_crossover, prob_mutacao);
	  char *melhor = NULL, 
	       *media = NULL,
	       *pior = NULL;

	  melhor = string_vec_double(melhor_global, geracoes);
	  media = string_vec_double(media_global, geracoes);
	  pior = string_vec_double(pior_global, geracoes);

	  molecule_string(molecula_otimizada, &molecule);

	  *energy = calcula_energia(molecula_otimizada);
     
	  asprintf(result, "%s\001%s\001%s\001%s", molecule, melhor, media, pior);

	  free(melhor);
	  free(media);
	  free(pior);

	  break;
     case 1:

	  otimizador(molecula_entrada, ITERACOES);

	  *energy = calcula_energia(molecula_entrada);

	  molecula_entrada->energy = *energy;



	  molecule_string(molecula_entrada, &molecule);

	  asprintf(result, "%s\001null\001null\001null", molecule);


	  
	  break;
     case 2:
	  *energy = calcula_energia(molecula_entrada);
	  molecula_entrada->energy = *energy;
	  molecule_string(molecula_entrada, &molecule);
	  asprintf(result, "%s\001null\001null\001null", molecule);

	  break;
     }

     free(molecule);
     /* free(melhor_global); */
     /* free(media_global); */
     /* free(pior_global); */

     if (molecula_entrada) destroy_molecule(molecula_entrada);
     if (molecula_otimizada) destroy_molecule(molecula_otimizada);

     hdestroy();
     return;
}
