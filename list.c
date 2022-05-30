#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "list.h"
#include <sys/socket.h>
#include <arpa/inet.h>

#define SIZE 1024


void ajouterEnTete(Liste *liste, int nvdSC, char* nvPseudo)
{
    /* On crée un nouvel élément */
    element* nouvelElement = malloc(sizeof(element));
 
    /* On assigne la valeur au nouvel élément */
    nouvelElement->dSC = nvdSC;

    strcpy(nouvelElement->pseudo, nvPseudo);
    nouvelElement->isConnected = 1;
    // nouvelElement->canal = "général";
 
    /* On assigne l'adresse de l'élément suivant au nouvel élément */
    nouvelElement->nxt = liste->head;
    liste->head = nouvelElement;
 
    /* On retourne la nouvelle liste, i.e. le pointeur sur le premier élément */
    // return nouvelElement;
}

void afficherListe(Liste *liste)
{
    element *tmp = liste->head;
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

/* ---- COMMANDE ---- */

void privateMessage(char* msg, int socket, Liste *liste) {
    char delim[] = " ";
    char *p = strtok(msg, delim);
    p = strtok(NULL, delim);
    char* p_desti = p;
    int dSC_desti = rechercherElementPseudo(liste, p)->dSC;

    char* msg_final = (char *) malloc(MAX_LENGTH);
    strcat(msg_final, "(privé) ");
    strcat(msg_final, p_desti);
    strcat(msg_final, " : ");
    p = strtok(NULL, delim);

    while ( p != NULL ) {
        strcat(msg_final, " ");
        strcat(msg_final, p);
        p = strtok(NULL, delim);
    }

    send(dSC_desti,msg_final,MAX_LENGTH,0);
    free(msg_final);
}

// display the commands list
void commandManual(int socket) {
    char* msg_final = "/man : display the commands list \n/mp username msg : send private message \n/list : display the list of connected client(s) \n/dc : disconnect \n";
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

void write_file(int sockfd){
    int n;
    FILE *fp;
    char *filename = "recv.txt";
    char buffer[SIZE];

    fp = fopen(filename, "w");
    while (1) {
        n = recv(sockfd, buffer, SIZE, 0);
        if (n <= 0){
            break;
            return;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }
    return;
}

void redirection(char* msg, int socket, Liste *liste) {
    int mp;
    int man;
    int list;
    int dc;
    int file;
    regex_t preg;
    const char *mp_regex = "^/mp";
    const char *man_regex = "^/man";
    const char *dc_regex = "^/dc";
    const char *list_regex = "^/list";
    const char *file_regex = "^/file";

    // Commande mp
    mp = regcomp (&preg, mp_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (mp == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            privateMessage(msg, socket, liste);
        }
    }

    // Commande man
    man = regcomp (&preg, man_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (man == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            commandManual(socket);
        }
    }

    // Commande dc
    dc = regcomp (&preg, dc_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (dc == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            disconnectClient(socket, liste);
        }
    }

    // Commande list
    list = regcomp (&preg, list_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if(list == 0){
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if(match == 0){
            envoyerListClients(socket, liste);
        }
    }

    // Commande file
    file = regcomp (&preg, file_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if(file == 0){
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if(match == 0){
            write_file(socket);
        }
    }
}


/*void envoyerListCanaux(int socket) {

}
*/