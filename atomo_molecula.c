#include <stdlib.h>
#include <string.h>
#include "atomo_molecula.h"

Atom *create_atom(char *elem, double x, double y, double z)
{
     Atom *tmp = calloc(1,sizeof(Atom));
     tmp->element = strdup(elem);
     tmp->x = x;
     tmp->y = y;
     tmp->z = z;

     return tmp;
}

Molecule *create_empty_molecule(int num_atomos)
{
     Molecule *retorno = calloc(1,sizeof(Molecule));
     retorno->molecule = (Atom **) calloc(num_atomos,sizeof(Atom **));
     retorno->num_atoms = num_atomos;
     return retorno;
}

Molecule *copy_molecule(Molecule *orig)
{
     int i;
     double x, y, z;
     char *elem;
     Atom *tmp_atom;
     Molecule *retorno = calloc(1,sizeof(Molecule));
     retorno->molecule = (Atom **) calloc(orig->num_atoms,sizeof(Atom **));
     retorno->num_atoms = orig->num_atoms;

     if (!orig->molecule)
     {
	  puts("Invalid Molecule");
	  exit(1);
     }

     for (i=0; i<orig->num_atoms; ++i)
     {
	  elem = orig->molecule[i]->element;
	  x = orig->molecule[i]->x;
	  y = orig->molecule[i]->y;
	  z = orig->molecule[i]->z;
	  tmp_atom = create_atom(elem, x, y, z);
	  retorno->molecule[i] = tmp_atom;
     }
     return retorno;

}


void destroy_atom(Atom *atom)
{
     if (atom->element)
     {
	  free(atom->element);
	  atom->element = NULL;
     }
     free(atom);
     atom = NULL;
}

void destroy_molecule(Molecule *molecule)
{
     int i;
     for (i=0; i<molecule->num_atoms; ++i)
     {
	  if (molecule->molecule[i])
	  {
	       destroy_atom(molecule->molecule[i]);
	       molecule->molecule[i] = NULL;
	  }
     }
     free(molecule->molecule);
     molecule->molecule = NULL;
     free(molecule);
     molecule = NULL;
}
