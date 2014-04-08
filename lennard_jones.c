#include <stdlib.h>
#include <stdio.h>
#include <search.h>
#include <string.h>
#include <math.h>

/* #include <gsl/gsl_multimin.h> */

#include "lennard_jones.h"
#include "atomo_molecula.h"
#include "simulador.h"

void process_string_potential(char *string_potential)
/* TODO: Optimize the function */
{

     int size = 1024;
     int pos, skip;
     int total_size, idx = 0;
     char c;

     char *buffer = (char *) malloc(size);

     double params[2];
     char elem[10];

     ENTRY e, *ep;

     total_size = strlen(string_potential);

     while (1) /* read all lines in file */
     {
	  pos = 0;
	  skip = 0;
	  do /* read one line */
	  {
	       c = string_potential[idx++];
	       if (c == '#') skip = 1; /* Mark the line to be discarded */
			 
	       if (c != EOF) buffer[pos++] = (char) c;
	       if(pos >= size - 1) { // increase buffer length - leave room for 0
		    size *= 2;
		    buffer = (char*) realloc(buffer, size);
	       }
		    
	  } while (c != '\n' && idx != total_size);



	  buffer[pos] = 0;	/* End of line */
	  //printf("%s", buffer);

	  if (skip) continue; /*  Skip the line */

	  /* Construct the param hash from the buffer */
	  sscanf(buffer, "%s %lf %lf", elem, &params[0], &params[1]);

	  e.key = strdup(elem);
	  double * entry = (double *) calloc(2, sizeof(double));
	  entry[0] = params[0];
	  entry[1] = params[1];
	  e.data = entry;
	       
		    
	  ep = hsearch(e, ENTER);
	  if (ep == NULL) 
	  {
	       fprintf(stderr, "entry failed in the parameter table\n");
	       exit(1);
	  }

	  if (idx == total_size) break;
     }

     free(buffer);
}

static void parameter_lennard_jones(const Atom *a, const Atom *b, double *param1)
{

     ENTRY e, *ep;
     //double param;

     char *potential = alloca(10);

     double * parameter;
     double um[2] = {1,1};
     
     sprintf(potential, "%s-%s", a->element, b->element);
     e.key = potential;
     ep = hsearch(e, FIND);

     if (ep) 
     {
	  parameter = ep->data;
     }
     else
     {
	  sprintf(potential, "%s-%s", b->element, a->element);
	  e.key = potential;
	  ep = hsearch(e, FIND);
	  if (ep) 
	  {
	       parameter = ep->data;
	  }
	  else
	  {
	       parameter = um;
	  }
     }

     param1[0] = parameter[0];
     param1[1] = parameter[1];

     return;
}

double potencial_lennard_jones_param(const double *a, const double *b, double *param)
{

     double x = (a[0] - b[0]);
     double y = (a[1] - b[1]);
     double z = (a[2] - b[2]);

     if (x == 0 && y == 0 && z == 0)
	  return 0;

     double distancia = sqrt((x*x) + (y*y) + (z*z));

     double c6, c12;//, e0;

     c12 = (double) param[0] / distancia;
//     printf("c12 = %f\n", c12);
     c6 = (double) param[1] / distancia;

     double p = 4*(pow((c12), 12) - pow((c6), 6));
     if (isnan(p))
     {
	  puts("isnan");
     	  return 0;
     }

     /* printf("p = %f\n", p); */
     return p;
}

double potencial_lennard_jones(const Atom *a, const Atom *b)
{
     double param[3];
     parameter_lennard_jones(a, b, param);

     double m[3], n[3];
     m[0] = a->x;
     m[1] = a->y;
     m[2] = a->z;

     n[0] = b->x;
     n[1] = b->y;
     n[2] = b->z;
     

     double p = potencial_lennard_jones_param(m, n, param);

     /* printf("p = %f\n", p); */
     return p;

}

double get_energia_molecula_lennard_jones(Molecule *molecula)
{
     double energia = 0;
     int i, j;
     for (i=0; i<molecula->num_atoms; ++i)
     {
	  for (j=i+1; j<molecula->num_atoms; ++j)
	  {
	       energia += potencial_lennard_jones(molecula->molecule[i], molecula->molecule[j]);
	  }
     }
     //printf("%f ", energia);
     return energia;
}

/* double my_f_lennard_jones (const gsl_vector *v, void *params) */
/* { */
/*      double *p = (double *)params; */
/*      int num_atomos = p[0]; */
/*      int num_interacoes=0; */
/*      double valor_potencial = 0; */
/*      int i, j; */
/*      for (i = 0; i < num_atomos; ++i)  */
/*      { */
/* 	  for (j = i+1; j < num_atomos; ++j)  */
/* 	  { */
/* 	       double a[] = {gsl_vector_get(v, (i*3)),gsl_vector_get(v, (i*3)+1),gsl_vector_get(v, (i*3)+2)}; */
/* 	       double b[] = {gsl_vector_get(v, (j*3)),gsl_vector_get(v, (j*3)+1),gsl_vector_get(v, (j*3)+2)}; */
/* 	       double param[] = {p[(num_interacoes*2)+1],p[(num_interacoes*2)+2]}; */
/* 	       valor_potencial += potencial_lennard_jones_param(a,b,param); */
/* 	       num_interacoes++; */

/* 	  } */
/*      } */
/*      return valor_potencial; */

/* } */




/* /\* The gradient of f, df = (df/dx, df/dy). *\/ */
/* void my_df_lennard_jones (const gsl_vector *v, void *params, gsl_vector *df) */
/* { */

/*      int i, j; */
/*      double *param_array = (double *)params; */

/*      int num_atomos =param_array[0]; */

/*      double x1_n; */
/*      double y1_n; */
/*      double z1_n; */

/*      double x2_n; */
/*      double y2_n; */
/*      double z2_n; */

/*      int num_interacoes=0; */
/*      //std::vector<double> df_n(num_atomos*3); */
/*      double *df_n = calloc(num_atomos*3, sizeof(double)); */

/*      for (i = 0; i < num_atomos; ++i) { */
/* 	  for (j = i+1; j < num_atomos; ++j) { */

/* 	       double C1 = param_array[(2*num_interacoes)+1]; */
/* 	       double C2 = param_array[(2*num_interacoes)+2]; */

/* 	       double x1 = gsl_vector_get(v,(i*3)); */
/* 	       double y1 = gsl_vector_get(v,(i*3)+1); */
/* 	       double z1 = gsl_vector_get(v,(i*3)+2); */

/* 	       double x2 = gsl_vector_get(v,(j*3)); */
/* 	       double y2 = gsl_vector_get(v,(j*3)+1); */
/* 	       double z2 = gsl_vector_get(v,(j*3)+2); */

/* 	       double dist =  pow( x1-x2,2) + pow( y1-y2,2) + pow( z1-z2, 2) ; */
/* 	       // NOVA POSICAO DO ATOMO A */
/* 	       x1_n = ((6 * (x1 - x2) * (-2*C1 + C2*(pow(dist,3))))) / (pow( dist ,7)); */
/* 	       y1_n = ((6 * (y1 - y2) * (-2*C1 + C2*(pow(dist,3))))) / (pow( dist ,7)); */
/* 	       z1_n = ((6 * (z1 - z2) * (-2*C1 + C2*(pow(dist,3))))) / (pow( dist ,7)); */

/* 	       // NOVA POSICAO DO ATOMO B */
/* 	       x2_n = (6 * (x1 - x2) * ( (2 * C1)- C2 * pow(dist,3))) / (pow(dist,7)); */
/* 	       y2_n = (6 * (y1 - y2) * ( (2 * C1)- C2 * pow(dist,3))) / (pow(dist,7)); */
/* 	       z2_n = (6 * (z1 - z2) * ( (2 * C1)- C2 * pow(dist,3))) / (pow(dist,7)); */

/* 	       // NOVA POSICAO DO ATOMO A */
/* 	       df_n[(i*3)]   += x1_n; */
/* 	       df_n[(i*3)+1] += y1_n; */
/* 	       df_n[(i*3)+2] += z1_n; */

/* 	       // NOVA POSICAO DO ATOMO B */
/* 	       df_n[(j*3)]   += x2_n; */
/* 	       df_n[(j*3)+1] += y2_n; */
/* 	       df_n[(j*3)+2] += z2_n; */

/* 	       num_interacoes++; */
/* 	  } */


/*      } */
/*      for (i = 0; i < num_atomos*3; ++i) { */
/* 	  gsl_vector_set(df,i,df_n[i]); */
/* 	  //		std::cout << df_n[i] << " "; */
/*      } */
/*      //	std::cout << std::endl; */
/*      free(df_n); */
/* } */


/* /\* Compute both f and df together. *\/ */
/* void my_fdf_lennard_jones (const gsl_vector *x, void *params,double *f, gsl_vector *df) */
/* { */
/*      *f = my_f_lennard_jones(x, params); */
/*      my_df_lennard_jones(x, params, df); */
/* } */



/* void executa_gradiente_lennard_jones(Molecule *molecula, double *param_vector, int num_iteracoes)  */
/* //void OtimizadorLocalLennardJones::executa_gradiente(std::vector<Atomo> *molecula,std::vector<double> *param_vector,int num_iteracoes) { */
/* { */
/*      int i, j; */

/*      gsl_multimin_function_fdf my_func; */
/*      my_func.n = molecula->num_atoms*3; */
/*      my_func.f = my_f_lennard_jones; */
/*      my_func.df = my_df_lennard_jones; */
/*      my_func.fdf = my_fdf_lennard_jones; */
/*      my_func.params = &param_vector[0]; */

/*      gsl_vector *x; */
/*      x = gsl_vector_calloc(molecula->num_atoms*3); */
/*      int num=0; */
/*      for (i = 0; i < (int)molecula->num_atoms; ++i) { */
/* 	  gsl_vector_set (x,num++, molecula->molecule[i]->x); */
/* 	  gsl_vector_set (x,num++, molecula->molecule[i]->y); */
/* 	  gsl_vector_set (x,num++, molecula->molecule[i]->z); */
/*      } */


/*      const gsl_multimin_fdfminimizer_type *T; */
/*      gsl_multimin_fdfminimizer *s; */


/*      T = gsl_multimin_fdfminimizer_conjugate_fr; */
/*      //		T = gsl_multimin_fdfminimizer_steepest_descent; */
/*      //	T = gsl_multimin_fdfminimizer_vector_bfgs; */
/*      s = gsl_multimin_fdfminimizer_alloc (T, my_func.n); */

/*      size_t iter = 0; */
/*      int status; */

/*      double step = 1; */

/*      gsl_multimin_fdfminimizer_set (s, &my_func, x, step, PRECISAO); */
/*      do */
/*      { */

/* 	  status = gsl_multimin_fdfminimizer_iterate(s); */
/* 	  if (status == GSL_ENOPROG){ */
/* 	       break; */
/* 	  } */
/* 	  status = gsl_multimin_test_gradient (s->gradient, PRECISAO); */

/*      } */
/*      while (status == GSL_CONTINUE && iter++ < 100); */

/*      for (i = 0; i < (int)molecula->num_atoms; ++i) { */
/* 	  Atom *atomo = molecula->molecule[i]; */
/* 	  atomo->x = (gsl_vector_get (s->x, (i*3))); */
/* 	  atomo->y = (gsl_vector_get (s->x, (i*3)+1)); */
/* 	  atomo->z = (gsl_vector_get (s->x, (i*3)+2)); */
/*      } */

/*      gsl_multimin_fdfminimizer_free (s); */

/*      gsl_vector_free (x); */
/* } */

/* void otimizar_lennard_jones(Molecule* molecula, int num_iteracoes)  */
/* { */

/*      // cria um vetor da molecula atual e a nova molecula depois do processo de otimizacao */
/*      // e a molecula atual que serah retornada */
/*      Molecule *nova_molecula = molecula; */
/*      double param[2]; */
/*      unsigned int counter = 1; */
/*      int i, j; */
/*      double preci = pow(0.1, PRECISAO); */

/*      /\*      Obtem todos os parametros      *\/ */
/*      double *param_vector = calloc(pow(molecula->num_atoms+1,2), sizeof(Atom)); */
/*      param_vector[0] = molecula->num_atoms; */
/*      for (i = 0; i < (int)molecula->num_atoms; ++i)  */
/*      { */
/* 	  for (j = i+1; j < (int)molecula->num_atoms; ++j)  */
/* 	  { */

/* 	       /\* std::stringstream oss; *\/ */
/* 	       /\* oss << ((Atomo)molecula->getMolecula().at(i)).getElemento() << "-" <<  ((Atomo)molecula->getMolecula().at(j)).getElemento(); *\/ */
/* 	       /\* double c1,c2; *\/ */
/* 	       /\* Parameter &p = Parameter::getInstance(); *\/ */
/* 	       /\* p.getParameter("LJ",oss.str(),"C1",c1); *\/ */
/* 	       /\* p.getParameter("LJ",oss.str(),"C2",c2); *\/ */
/* 	       //			std::cout << "\n"<< oss.str() << "\t" << c1 << ":" << c2 << std::endl; */

/* 	       Atom *a = molecula->molecule[i]; */
/* 	       Atom *b = molecula->molecule[j]; */
/* 	       parameter_lennard_jones(a, b, (double *) &param); */

/* 	       param_vector[counter++] = param[0]; */
/* 	       param_vector[counter++] = param[1]; */

/* 	  } */
/*      } */

/*      int iter=0; */
/*      double energia_atual,nova_energia; */
/*      //   get_energia_molecula_lennard_jones(nova_molecula); */
/*      do{ */


/* 	  // obtem a energia da molecula atual */
/* 	  energia_atual = get_energia_molecula_lennard_jones(nova_molecula); */

/* 	  executa_gradiente_lennard_jones(nova_molecula, param_vector,num_iteracoes); */

/* 	  // obtem a energia da nova molecula */

/* 	  nova_energia = get_energia_molecula_lennard_jones(nova_molecula); */
/* 	  //std::cout << iter <<"\tEnergia Anterior:" << energia_atual << "\tNova Energia:" << nova_energia << std::endl; */
/* 	  ++iter; */
/*      } */
/*      // repete enquando a energia da nova molecula seja menor que a atual */
/* //     while(nova_energia > (energia_atual + preci) && iter < num_iteracoes); */
/*      while(iter < num_iteracoes); */
/*      free(param_vector); */
/*      return; */
/*      // retorna a molecula atual */

/*      /\* Molecule molecula_retorno; *\/ */
/*      /\* molecula_retorno.setMolecula(nova_molecula); *\/ */
/*      /\* //		std::cout  << "Molecula Otimizada :\n" <<  potencial->getEnergiaMolecula(molecula_retorno) << std::endl; *\/ */
/*      /\* return molecula_retorno; *\/ */
/* } */

void otimizar_lennard_jones(Molecule* molecula, int num_iteracoes)
{
     return;
}
