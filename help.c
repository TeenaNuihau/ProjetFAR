#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <pthread.h>
#include "help.h"
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
        strcat(msg_final, "[privé] ");
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
    char* commandes = (char*) malloc(MAX_LENGTH);
    strcat(commandes, "---Liste des commandes : \n");
    sprintf(commandes, "%s %s \n",commandes, "/man : display the command manual");
    sprintf(commandes, "%s %s \n",commandes, "/mp <username> : send private message");
    sprintf(commandes, "%s %s \n",commandes, "/kick <username> : disconnect the corresponding user" );
    sprintf(commandes, "%s %s \n",commandes, "/dc : disconnect from the server" );
    sprintf(commandes, "%s %s \n",commandes, "/list : display all clients connected");
    sprintf(commandes, "%s %s \n",commandes, "/canallist : display clients connected to the same channel");
    sprintf(commandes, "%s %s \n",commandes, "/canalcr : create new channel");
    sprintf(commandes, "%s %s \n",commandes, "/canalcg <channelNumber> : connect to the channel which number is given");
    sprintf(commandes, "%s %s \n",commandes, "/canals : display the list of channels");
    sprintf(commandes, "%s %s \n",commandes, "/canaldc : disconnect you from current channel" );
    sprintf(commandes, "%s %s \n",commandes, "/canaldl : delete current channel and redirect to Channel 0 the clients connected to the channel" );
    sprintf(commandes, "%s %s \n",commandes, "/listfile : display the files you can upload" );
    sprintf(commandes, "%s %s \n",commandes, "/file <fileNumber> : send the file from the directory clientFiles/ which number is specified" );
    sprintf(commandes, "%s %s \n",commandes, "/dllist : display the files you can download" );
    sprintf(commandes, "%s %s \n",commandes, "/dlfile <fileNumber> : download the file which number is given in the directory downloads/" );
    sprintf(commandes, "%s %s \n",commandes, "/color : change input message color" );
    sprintf(commandes, "%s %s \n",commandes, "\t 0 -> white (default)" );
    sprintf(commandes, "%s %s \n",commandes, "\t 1 -> blue" );
    sprintf(commandes, "%s %s \n",commandes, "\t 2 -> green" );
    sprintf(commandes, "%s %s \n",commandes, "\t 3 -> orange" );
    sprintf(commandes, "%s %s \n",commandes, "\t 4 -> cyan" );
    sprintf(commandes, "%s %s \n",commandes, "\t 5 -> red" );
    send(socket, commandes, MAX_LENGTH, 0);
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
        strcat(msg_final," Canal : ");
        char* canal = (char *) malloc(10);
        sprintf(canal, "%d", tmp->canal);
        strcat(msg_final,canal);
        strcat(msg_final," \n");
        tmp = tmp->nxt;
    }

    send(socket,msg_final,MAX_LENGTH,0);
}

void envoyerListClientsCanal(int socket,int canal, Liste *liste) {
    char* msg_final = (char *) malloc(MAX_LENGTH*MAX_CLIENTS+200);
    sprintf(msg_final,"Liste des clients dans le canal %d : \n",canal);
    element* tmp = liste->head;
    while (tmp!=NULL) {
        if(tmp->canal == canal){
            strcat(msg_final,tmp->pseudo);
            strcat(msg_final," \n");
        }
        tmp = tmp->nxt;
    }
    send(socket,msg_final,MAX_LENGTH,0);
}
