#include <stdlib.h>
#include <stdio.h>

#include "simulador.h"
#include "atomo_molecula.h"
#include "ga.h"

Molecule *otimizar_ga(Molecule *molecula, int geracoes, int tam_populacao, int prob_mutacao, int prob_crossover)
{
	/* parametros do GA */

	double tamanho_arestas = 40; // tamanho máximo das arestas
	int i, j;

	Agregado *geracao_ascendente, *geracao_descendente;


	/* If we have an odd populations, make it even. */
	if (tam_populacao % 2 != 0) ++tam_populacao;


	/* Cria as posicoes em memoria para as populacoes */
	geracao_ascendente = create_empty_agregado(tam_populacao);

	     
	//printf(">>>>>> Cria populacao inicial\n");
	cria_populacao_inicial(geracao_ascendente, molecula, tam_populacao,tamanho_arestas);

	
	for (i = 0; i < geracoes ; ++i )
	{
		printf(">> Geracao %d\n", i);
		geracao_descendente = cria_populacao_descendente(prob_mutacao,prob_crossover,tamanho_arestas, geracao_ascendente);


		/* Estatisticas globais */
		melhor_global[i] = geracao_descendente->agregado[0]->energy;
		pior_global[i] = geracao_descendente->agregado[tam_populacao-1]->energy;
		double tmp_media = 0;
		for (j=0; j<tam_populacao; ++j)
		{
		     tmp_media += geracao_descendente->agregado[j]->energy;
		}
		media_global[i] = tmp_media/tam_populacao;
		/************************/
		geracao_ascendente = geracao_descendente;
	}

	return geracao_descendente->agregado[0];
}

void cria_populacao_inicial(Agregado *geracao_ascendente, Molecule *recebida, int tamanho_populacao, double tamanho_arestas)
{
	int i;
	for (i = 0; i < tamanho_populacao; ++i)
	{
	     Molecule *aleatoria = gera_molecula_aleatoria(recebida, tamanho_arestas);
	     calcula_energia_molecula(aleatoria);
	     geracao_ascendente->agregado[i] = aleatoria;
	}
	ordena(geracao_ascendente);
}



Agregado *cria_populacao_descendente(int Pm, int Pc, double tamanho_arestas, Agregado *geracao_ascendente)
{
     int i;
     int tam_populacao = geracao_ascendente->num_molecules;

     Agregado *geracao_descendente = create_empty_agregado(tam_populacao);

     Molecule *ind1;
     Molecule *ind2;

     /* TODO: Verificar de onde esta pegando os pais */

     // cria nova geracao de individuos
     for(i=0; i<tam_populacao; i+=2)
     {
	  //ordena(geracao_ascendente);
	  int result;
	  do
	  {
	       result = rank(geracao_ascendente, &ind1, &ind2); // seleciona pais
	  } while (!result);
     
	  // crossover
	  if((rand()%100)<Pc)
	  {
	       crossover(ind1, ind2);
	  }

     	  // mutacao
	  if((rand()%100)<Pm)
	  {
	       mutacao(ind1,tamanho_arestas);
	       mutacao(ind2,tamanho_arestas);
	  }
	


	  otimizador(ind1,100);
	  otimizador(ind2,100);
	  
	  double fit = calcula_energia_molecula(ind1);
	  ind1->energy = fit;
	  fit = calcula_energia_molecula(ind2);
	  ind2->energy = fit;
	  
	  
	  geracao_descendente->agregado[i  ] = ind1;
	  geracao_descendente->agregado[i+1] = ind2;
     }

     /* Coloca o melhor na proxima geracao - Elitismo */
     geracao_descendente->agregado[0] = geracao_ascendente->agregado[0];
//     geracao_ascendente->agregado[0] = NULL;
     for (i=0; i<tam_populacao; ++i)
     {
	     
	  Molecule *avaliada = geracao_descendente->agregado[i];
	  calcula_energia_molecula(avaliada);

     }
     ordena(geracao_descendente);
     /* TODO: Destruir geracao ascendente aqui ou fora da funcao.*/
     return geracao_descendente;
}






/* int getIndiceMelhorFitness() */
/* { */
/* 	int i; */
/* 	int melhor = 0; */
/* 	double fitnessMedia = geracaoAscendente[0].getFitness(); */
/* 	for(i = 1; i<(int)geracaoAscendente.size(); i++) { */
/* 		fitnessMedia += geracaoAscendente[i].getFitness(); */
/* 		if ( geracaoAscendente[i].getFitness() < geracaoAscendente[melhor].getFitness()) { */
/* 			melhor = i; */
/* 		} */
/* 	} */
/* 	fitnessMedia /= geracaoAscendente.size(); */


/* 	return melhor; */
/* } */



/* Molecula getMelhorMolecula() */
/* { */
/* 	int melhor = getIndiceMelhorFitness(); */
/* 	return geracaoAscendente[melhor]; */
/* } */



double gera_coordenada_aleatoria(double tamanho_aresta)
{
	double precisao = 10000000;
	return (rand()%(int)(tamanho_aresta*precisao))/precisao;
}



Molecule *gera_molecula_aleatoria(Molecule *recebida, double tamanho_arestas)
{
	int i;
	double x, y, z;
	char *elem;
	Molecule *criadora = create_empty_molecule(recebida->num_atoms);

	Atom *cria_atomo;

	for (i = 0; i < (int)recebida->num_atoms; ++i ) 
	{
	     x = gera_coordenada_aleatoria(tamanho_arestas);
	     y = gera_coordenada_aleatoria(tamanho_arestas);
	     z = gera_coordenada_aleatoria(tamanho_arestas);
	     elem = recebida->molecule[i]->element;
	     
	     cria_atomo = create_atom(elem, x, y, z);
	     criadora->molecule[i] = cria_atomo;
	}

	return criadora;
}



void crossover(Molecule *ind1, Molecule *ind2)
{
	int posicao_crossover;
	int i;
	int tamanho_molecula;

	tamanho_molecula = ind1->num_atoms;

	posicao_crossover = rand()%tamanho_molecula;
	Atom *tmp_atom;

	/* Molecule *auxiliar_ind1 = copy_molecule(ind1); */
	/* Molecule *auxiliar_ind2 = copy_molecule(ind2); */


	for (i = posicao_crossover; i < tamanho_molecula; ++i)
	{
	     tmp_atom = ind1->molecule[i];
	     ind1->molecule[i] = ind2->molecule[i];
	     ind2->molecule[i] = tmp_atom;
	}
}



void mutacao(Molecule *molecula, double tamanho_arestas)
{
	int i;
	double x, y, z;

	int chance = rand() % 3;

	if (chance == 0)
	{
	     /* Alguns atomos tem suas coordenadas deslocadas */
	     int max = (int) (rand() % molecula->num_atoms)-1;
	     for (i=0; i<max ; ++i)
	     {

		  x = gera_coordenada_aleatoria(tamanho_arestas);
		  y = gera_coordenada_aleatoria(tamanho_arestas);
		  z = gera_coordenada_aleatoria(tamanho_arestas);
	     
//		  printf("x = %f, y = %f, z = %f\n", x, y, z);

		  molecula->molecule[i]->x = x;
		  molecula->molecule[i]->y = y;
		  molecula->molecule[i]->z = z;
	     }
		  
	}
	else if (chance == 1)
	{
	     /* Agregado eh substituido completamente */
	     /* Molecule *aleatoria = gera_molecula_aleatoria(molecula, tamanho_arestas); */
	     /* destroy_molecule(molecula); */
	     /* molecula = aleatoria; */
	}

	else if (chance == 2)
	{
	     /* Troca a posicao de dois atomos */
	     Atom *tmp_atom;
	     int pos1 = rand()%molecula->num_atoms;
	     int pos2 = rand()%molecula->num_atoms;
	     tmp_atom = molecula->molecule[pos1];
	     molecula->molecule[pos1] = molecula->molecule[pos2];
	     molecula->molecule[pos2] = tmp_atom;
	}
}





double calcula_energia_molecula(Molecule *molecula)
{
	double energia = 0;
	energia = calcula_energia(molecula);
	molecula->energy = energia;

	return energia;
}

void ordena(Agregado *original)
{
     int tamanho = original->num_molecules;
     int i, j;
     Molecule *tmp;

     /* Ordena o agregado em ordem de menor energia (maior fitness) */
     for (i=0; i<tamanho; ++i)
     {
	  for (j=i+1; j<tamanho; ++j)
	  {
	       if (original->agregado[j]->energy < original->agregado[i]->energy)
	       {
		    tmp = original->agregado[i];
		    original->agregado[i] = original->agregado[j];
		    original->agregado[j] = tmp;
	       }
	  }
     }

}     

int rank(Agregado *original, Molecule **ind1t, Molecule **ind2t)
{
     int tamanho = original->num_molecules;
     int i;

     int counter = 0;
     for (i=0; i<tamanho; ++i)
     {
	  double prob = (double) rand()/RAND_MAX;
	  double chance = (double) (tamanho - i)/(tamanho*2);
	  if (chance < prob)
	  {
	       if (counter == 0)
	       {
		    *ind1t = copy_molecule(original->agregado[i]);
		    ++counter;
	       }
	       else if (counter == 1)
	       {
		    *ind2t = copy_molecule(original->agregado[i]);
		    ++counter;
	       }
	       else
	       {
		    return 1;
	       }
	  }
     }
     return 0;
}

Agregado *create_empty_agregado(int tamanho)
{

     Agregado *tmp = calloc(1,sizeof(Agregado));
     tmp->num_molecules = tamanho;
     tmp->agregado = (Molecule **) calloc(tamanho, sizeof(Molecule **));
     return tmp;
}

void destroy_agregado(Agregado *a)
{
     int i,tamanho = a->num_molecules;
     for (i=0; i<tamanho; ++i)
     {
	  destroy_molecule(a->agregado[i]);
     }
     free(a->agregado);
     free(a);
}
