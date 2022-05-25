//
// Created by Romain Mezghenna, Mathieu Dos Santos, Nuihau Teena on 7/04/2022.
//

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <regex.h>
#include "list.h"


#define PORT 3004 //Port of the server


Liste *listeClient;
sem_t semaphore;

void ajouterClient(int socket, char* pseudo){
    sem_wait(&semaphore);
    ajouterEnTete(listeClient, socket, pseudo);
    sem_post(&semaphore);
}

void supprimerClient(int socket){
    sem_wait(&semaphore);
    supprimerElement(listeClient, socket);
    sem_post(&semaphore);
}

void envoyerMessageGeneral(char* msg, int from){
    element* tmp = listeClient->head;
    while(tmp != NULL && tmp->isConnected == 1){
        if(tmp->dSC != from){
            send(tmp->dSC, msg, MAX_LENGTH, 0);
        }
        tmp = tmp->nxt;
    }
}

void traitementClient(element* client){
    char msg[MAX_LENGTH];
    printf("Client %d connected\n", client->dSC);
    while(client->isConnected == 1){
        if(recv(client->dSC, msg, MAX_LENGTH, 0)!=-1){
            printf("Message reçu de %s : %s\n", client->pseudo, msg);
            if(msg[0]!='/'){ // Not a command
                char* msg_final = (char *) malloc(MAX_LENGTH);
                strcat(msg_final, client->pseudo);
                strcat(msg_final, " : ");
                strcat(msg_final, msg);
                envoyerMessageGeneral(msg_final, client->dSC);
            } else { // It's a command
                redirection(msg, client->dSC, listeClient);
            }
        }

    }

}




int main(int argc, char** argv){
    listeClient = creerListe();
    printf("Début programme\n");

    int dS = socket(PF_INET, SOCK_STREAM, 0);
    printf("Socket Créé\n");

    struct sockaddr_in ad;
    ad.sin_family = AF_INET;
    ad.sin_addr.s_addr = INADDR_ANY ;
    ad.sin_port = htons(PORT);
    bind(dS, (struct sockaddr*)&ad, sizeof(ad)) ;
    printf("Socket Nommé\n");

    listen(dS, 7) ;
    printf("Mode écoute\n");

    struct sockaddr_in aC ;
    socklen_t lg = sizeof(struct sockaddr_in) ;

    pthread_t thread[MAX_CLIENTS];
    sem_init(&semaphore, 0, 1);


    while(listeClient->nbElements < MAX_CLIENTS){
        afficherListe(listeClient);
        int dSC = accept(dS, (struct sockaddr*)&aC, &lg);
        printf("Client connecté\n");
        // Infos Client
        char* pseudoR = (char *) malloc(MAX_LENGTH);
        recv(dSC, pseudoR, MAX_LENGTH, 0);
        size_t taille = strlen(pseudoR)-1;
        char pseudo[taille];
        strncpy(pseudo,pseudoR,(size_t)(taille));
        pseudo[taille]='\0';
        printf("Pseudo reçu : %s \n",pseudo);
        ajouterClient(dSC, pseudo);
        printf("Client ajouté\n");
        pthread_create(&thread[listeClient->nbElements - 1], NULL, traitementClient, listeClient->head);
        printf("Thread créé\n");
    }




    return 0;

}