#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include <sys/socket.h>
#include <arpa/inet.h>


llist ajouterEnTete(llist liste, int nvdSC, char* nvPseudo)
{
    /* On crée un nouvel élément */
    element* nouvelElement = malloc(sizeof(element));
 
    /* On assigne la valeur au nouvel élément */
    nouvelElement->dSC = nvdSC;

    strcpy(nouvelElement->pseudo, nvPseudo);
    nouvelElement->isConnected = 1;
    // nouvelElement->canal = "général";
 
    /* On assigne l'adresse de l'élément suivant au nouvel élément */
    nouvelElement->nxt = liste;
 
    /* On retourne la nouvelle liste, i.e. le pointeur sur le premier élément */
    return nouvelElement;
}

void afficherListe(llist liste)
{
    element *tmp = liste;
    /* Tant que l'on n'est pas au bout de la liste */
    printf("Afficher list : \n");
    while(tmp != NULL)
    {
        /* On affiche */
        printf("%s", tmp->pseudo);
        /* On avance d'une case */
        tmp = tmp->nxt;
    }
    printf("\n");
}

int estVide(llist liste)
{
    if(liste.head == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

element* rechercherElementSocket(Liste *liste, int socket)
{
    element *tmp=liste->head;
    /* Tant que l'on n'est pas au bout de la liste */
    while(tmp != NULL)
    {
        if(tmp->dSC == socket)
        {
            /* Si l'élément a la valeur recherchée, on renvoie son adresse */
            return tmp;
        }
        tmp = tmp->nxt;
    }
    return NULL;
}

element* rechercherElementPseudo(Liste *liste, char* pseudo){
    element *tmp=liste->head;
    /* Tant que l'on n'est pas au bout de la liste */
    while(tmp != NULL)
    {
        if(tmp->pseudo == pseudo)
        {
            /* Si l'élément a la valeur recherchée, on renvoie son adresse */
            return tmp;
        }
        tmp = tmp->nxt;
    }
    return NULL;
}

void supprimerElement(Liste *liste, int valeur){
    element *tmp = liste->head;
    element *prec = NULL;
    /* Tant que l'on n'est pas au bout de la liste */
    while(tmp != NULL)
    {
        if(tmp->dSC == valeur)
        {
            tmp->isConnected = 0;
            /* Si l'élément a la valeur recherchée, on supprime l'élément */
            if(prec == NULL)
            {
                liste->head = tmp->nxt;
            }
            else
            {
                prec->nxt = tmp->nxt;
            }
            free(tmp);
            liste->nbElements -= 1;
            return;
        }
        prec = tmp;
        tmp = tmp->nxt;
    }
}
/*
void changerCanal(Liste *liste, int socket, char* nvCanal) {
    element* tmp = liste->head;
    while (tmp != NULL) {
        if (tmp->dSC!=socket) {
            tmp->canal = nvCanal;
        }
    }
}
*/
Liste* creerListe()
{
    Liste *liste = (Liste*)malloc(sizeof(Liste));
    liste->head = NULL;
    liste->nbElements = 0;
    return liste;
}