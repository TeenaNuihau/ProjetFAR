#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 100
#define MAX_CLIENTS 50

typedef struct element element;
struct element
{
    int dSC;
    char pseudo[MAX_LENGTH];
    // char* canal;
    int isConnected;
    struct element *nxt;
};

typedef struct Liste Liste;
struct Liste {
    element* head;
    int nbElements;
};

void ajouterEnTete(Liste *liste, int nvdSC, char* nvPseudo);

void afficherListe(Liste *liste);

int estVide(Liste liste);

element* rechercherElementSocket(Liste *liste, int socket);

element* rechercherElementPseudo(Liste *liste, char* pseudo);

void supprimerElement(Liste *liste, int valeur);

// void changerCanal(Liste *liste, int socket, char* nvCanal);

Liste* creerListe();

/* ---- COMMANDE ---- */

int trouverPseudo(char* pseudo, Liste *liste);

void privateMessage(char* msg, int socket, Liste *liste);

void commandManual(int socket);

void disconnectClient(int socket, Liste *liste);

void envoyerListClients(int socket, Liste *liste);

void envoyerListCanaux(int socket);

void redirection(char* msg, int socket, Liste *liste);