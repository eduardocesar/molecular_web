#ifndef LENNARD_JONES_H
#define LENNARD_JONES_H

#include "atomo_molecula.h"

void le_potencial_lennard_jones();
double potencial_lennard_jones(const Atom *a, const Atom *b);
double potencial_lennard_jones_param(const double *a, const double *b, double *param_array);
void otimizar_lennard_jones(Molecule* molecula, int num_iteracoes);
double get_energia_molecula_lennard_jones(Molecule *molecula);

#endif	/* LENNARD_JONES_H */
