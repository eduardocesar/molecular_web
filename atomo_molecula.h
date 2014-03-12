#ifndef ATOMO_MOLECULA_H
#define ATOMO_MOLECULA_H

typedef struct
{
     char *element;
     double x;
     double y;
     double z;
} Atom;

typedef struct
{
     Atom ** molecule;
     unsigned int num_atoms;
     double energy;
     
} Molecule;




Atom *create_atom(char *elem, double x, double y, double z);
Molecule *create_empty_molecule(int num_atomos);
Molecule *copy_molecule(Molecule *orig);
void destroy_atomo(Atom *atom);
void destroy_molecule(Molecule *molecule);

#endif /* ATOMO_MOLECULA_H */
