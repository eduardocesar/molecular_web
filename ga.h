#ifndef GA_H
#define GA_H

#include "atomo_molecula.h"

typedef struct
{
     Molecule ** agregado;
     unsigned int num_molecules;
} Agregado;

Molecule *otimizar_ga(Molecule *molecula, int geracoes, int tam_populacao, int prob_crossover, int prob_mutacao);

Molecule *gera_molecula_aleatoria(Molecule *recebida, double tamanho_arestas);

void cria_populacao_inicial(Agregado *geracao_ascendente, Molecule *recebida, int tamanho_populacao, double tamanho_arestas);

Agregado *cria_populacao_descendente(int Pm, int Pc, double tamanhoArestas, Agregado *geracao_ascendente);

void crossover(Molecule *ind1, Molecule *ind2);

void mutacao(Molecule *molecula, double tamanho_arestas);

void ordena(Agregado *original);

int rank(Agregado *original, Molecule **ind1, Molecule **ind2);

double calcula_energia_molecula(Molecule *molecula);

Agregado *create_empty_agregado(int tamanho);

void destroy_agregado(Agregado *a);
#endif /* GA_H */
