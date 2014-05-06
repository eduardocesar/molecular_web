#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <search.h>
#include "atomo_molecula.h"

#define PRECISAO 10
#define ITERACOES 100

enum {MORSE = 1, LJ, GUPTA};

double (*potencial)(const Atom *a, const Atom *b);

void (*otimizador)(Molecule *molecule, int num_iteracoes);

double (*calcula_energia)(Molecule *molecula);

double *melhor_global, *pior_global, *media_global;

int line_counter(char *s);

void newmain(char *params, char **result, double *energy);

#endif	/* SIMULADOR_H */
