#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <search.h>
#include "atomo_molecula.h"

#define ARQUIVO_POTENCIAL_LENNARD_JONES "parametros_lennard_jones.par"
#define ARQUIVO_POTENCIAL_MORSE "parametros_morse.par"
#define ARQUIVO_POTENCIAL_GUPTA "parametros_gupta.par"

#define PRECISAO 10


/* Algoritmo Genetico */


enum {MORSE = 1, LJ, GUPTA};

double (*potencial)(const Atom *a, const Atom *b);

void (*otimizador)(Molecule *molecule, int num_iteracoes);

double (*calcula_energia)(Molecule *molecula);

double *melhor_global, *pior_global, *media_global;

int line_counter(char *s);

void newmain(char *params);

#endif	/* SIMULADOR_H */
