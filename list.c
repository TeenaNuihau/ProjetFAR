#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <pthread.h>
#include "list.h"
#include <sys/socket.h>
#include <arpa/inet.h>


void ajouterEnTete(Liste *liste, int nvdSC, char* nvPseudo)
{
    /* On crée un nouvel élément */
    element* nouvelElement = malloc(sizeof(element));
 
    /* On assigne la valeur au nouvel élément */
    nouvelElement->dSC = nvdSC;

    strcpy(nouvelElement->pseudo, nvPseudo);
    nouvelElement->isConnected = 1;
    nouvelElement->canal = 0; // canal général
 
    /* On assigne l'adresse de l'élément suivant au nouvel élément */
    nouvelElement->nxt = liste->head;
    liste->head = nouvelElement;
 
    /* On retourne la nouvelle liste, i.e. le pointeur sur le premier élément */
    // return nouvelElement;
}

void afficherListe(Liste *liste)
{
    printf("Afficher list : \n");
    element *tmp = liste->head;
    /* Tant que l'on n'est pas au bout de la liste */
    while(tmp != NULL)
    {
        /* On affiche */
        printf("%s", tmp->pseudo);
        /* On avance d'une case */
        tmp = tmp->nxt;
    }
    printf("\n");
}

int estVide(Liste liste)
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
        if(strcmp(tmp->pseudo,pseudo) == 0)
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
    while(tmp != NULL){
        if(tmp->dSC == valeur){
            printf("Client : %s déconnecté",tmp->pseudo);
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

void changerCanal(Liste *liste, int socket, int nvCanal) {
    element* tmp = liste->head;
    while (tmp != NULL) {
        if (tmp->dSC!=socket) {
            tmp->canal = nvCanal;
        }
    }
}

Liste* creerListe()
{
    Liste *liste = (Liste*)malloc(sizeof(Liste));
    liste->head = NULL;
    liste->nbElements = 0;
    return liste;
}

/* ---- COMMANDE ---- */

void privateMessage(char* msg, int socket, Liste *liste) {
    char delim[] = " ";
    char *p = strtok(msg, delim);
    p = strtok(NULL, delim);
    char* p_desti = p;
    struct element* desti = rechercherElementPseudo(liste, p);
    int dSC_desti;

    char* pseudoEmeteur = rechercherElementSocket(liste, socket)->pseudo;

    if(desti != NULL) {
        dSC_desti = desti->dSC;
        char *msg_final = (char *) malloc(MAX_LENGTH);
        strcat(msg_final, "(privé) ");
        strcat(msg_final, pseudoEmeteur);
        strcat(msg_final, " : ");
        p = strtok(NULL, delim);

        while (p != NULL) {
            strcat(msg_final, " ");
            strcat(msg_final, p);
            p = strtok(NULL, delim);
        }

        send(dSC_desti, msg_final, MAX_LENGTH, 0);
        free(msg_final);
    } else {
        printf("Pseudo inconnu : %s\n", p_desti);
        char *msg_final = (char *) malloc(MAX_LENGTH);
        strcat(msg_final, "Pseudo inconnu");
        send(socket, msg_final, MAX_LENGTH, 0);
        free(msg_final);
    }
}

// display the commands list
void commandManual(int socket) {
    char* msg_final = "/man : display the commands list \n/mp <username> msg : send private message \n/list : display the list of connected client(s) \n/dc : disconnect \n/file <fileNumber> : send file\n/dllist : display all the downloadable files\n";
    send(socket, msg_final, MAX_LENGTH, 0);
}

void disconnectClient(int socket, Liste *liste) {
    supprimerElement(liste, socket);
    shutdown(socket, 2);
}

void envoyerListClients(int socket, Liste *liste) {
    char* msg_final = (char *) malloc(MAX_LENGTH*MAX_CLIENTS+200);
    strcat(msg_final,"Liste des clients : \n");

    element* tmp = liste->head;
    while (tmp!=NULL) {
        strcat(msg_final,tmp->pseudo);
        strcat(msg_final," \n");
        tmp = tmp->nxt;
    }

    send(socket,msg_final,MAX_LENGTH,0);
}



/*
 * void creerCanal() {
 * }
 *
 * void envoyerListCanaux(int socket) {

*}
*/