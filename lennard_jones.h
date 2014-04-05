#ifndef LENNARD_JONES_H
#define LENNARD_JONES_H

#include "atomo_molecula.h"

void process_string_potential(char *string_potential);
double potencial_lennard_jones(const Atom *a, const Atom *b);
double potencial_lennard_jones_param(const double *a, const double *b, double *param_array);
double get_energia_molecula_lennard_jones(Molecule *molecula);
void otimizar_lennard_jones(Molecule* molecula, int num_iteracoes);

#endif	/* LENNARD_JONES_H */
