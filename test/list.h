#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct element element;
struct element
{
    int dSC;
    char* pseudo;
    struct element *nxt;
};
 
typedef element* llist;

llist ajouterEnTete(llist liste, int nvdSC, char* nvPseudo);

void afficherListe(llist liste);

int estVide(llist liste);

llist rechercherElement(llist liste, int valeur);

llist supprimerElement(llist liste, int valeur);

int nombreElements(llist liste);