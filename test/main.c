#include <stdlib.h>
#include <stdio.h>

#include "list.h"

int main(int argc, char **argv)
{
        llist ma_liste1;
        ma_liste1 = ajouterEnTete(ma_liste1, 1, "nui");
        ma_liste1 = ajouterEnTete(ma_liste1, 2, "mat");
        ma_liste1 = ajouterEnTete(ma_liste1, 3, "rom");
        ma_liste1 = ajouterEnTete(ma_liste1, 4, "rob");
        ma_liste1 = supprimerElement(ma_liste1, 3);

        afficherListe(rechercherElement(ma_liste1, 1));
 
 
    return 0;
}