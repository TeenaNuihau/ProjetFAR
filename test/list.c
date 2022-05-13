#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

llist ajouterEnTete(llist liste, int nvdSC, char* nvPseudo)
{
    /* On crée un nouvel élément */
    element* nouvelElement = malloc(sizeof(element));
 
    /* On assigne la valeur au nouvel élément */
    nouvelElement->dSC = nvdSC;
    nouvelElement->pseudo = nvPseudo;
 
    /* On assigne l'adresse de l'élément suivant au nouvel élément */
    nouvelElement->nxt = liste;
 
    /* On retourne la nouvelle liste, i.e. le pointeur sur le premier élément */
    return nouvelElement;
}

void afficherListe(llist liste)
{
    element *tmp = liste;
    /* Tant que l'on n'est pas au bout de la liste */
    while(tmp != NULL)
    {
        /* On affiche */
        printf("%s ", tmp->pseudo);
        /* On avance d'une case */
        tmp = tmp->nxt;
    }
    printf("\n");
}

int estVide(llist liste)
{
    if(liste == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

llist rechercherElement(llist liste, int valeur)
{
    element *tmp=liste;
    /* Tant que l'on n'est pas au bout de la liste */
    while(tmp != NULL)
    {
        if(tmp->dSC == valeur)
        {
            /* Si l'élément a la valeur recherchée, on renvoie son adresse */
            return tmp;
        }
        tmp = tmp->nxt;
    }
    return NULL;
}

llist supprimerElement(llist liste, int valeur)
{
    /* Liste vide, il n'y a plus rien à supprimer */
    if(liste == NULL)
        return NULL;
 
    /* Si l'élément en cours de traitement doit être supprimé */
    if(liste->dSC == valeur)
    {
        /* On le supprime en prenant soin de mémoriser 
        l'adresse de l'élément suivant */
        element* tmp = liste->nxt;
        free(liste);
        /* L'élément ayant été supprimé, la liste commencera à l'élément suivant
        pointant sur une liste qui ne contient plus aucun élément ayant la valeur recherchée */
        tmp = supprimerElement(tmp, valeur);
        return tmp;
    }
    else
    {
        /* Si l'élement en cours de traitement ne doit pas être supprimé,
        alors la liste finale commencera par cet élément et suivra une liste ne contenant
        plus d'élément ayant la valeur recherchée */
        liste->nxt = supprimerElement(liste->nxt, valeur);
        return liste;
    }
}

int nombreElements(llist liste)
{
    int nb = 0;
    element* tmp = liste;
 
    /* On parcours la liste */
    while(tmp != NULL)
    {
        /* On incrémente */
        nb++;
        tmp = tmp->nxt;
    }
    /* On retourne le nombre d'éléments parcourus */
    return nb;
}