//
// Created by Romain Mezghenna, Mathieu Dos Santos, Nuihau Teena on 7/04/2022.
//

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <regex.h>
#include "list.h"
#include <dirent.h>


#define PORT 3002 //Port of the server


Liste *listeClient;
sem_t semaphore;
pthread_t fileThread[20];
int nbFileThread = 0;
int dSocketFile;
char* mostRecentFile;

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

char* getfilename(int n) {
    char* filename = malloc(sizeof(char) * MAX_LENGTH);
    DIR *d = opendir("./serverFiles");
    struct dirent *dir;
    int cpt=1;
    if (d) {
        while ((dir = readdir(d)) != NULL){
            if(dir->d_type==8){
                if(cpt==n){
                    filename=dir->d_name;
                }
                cpt++;
            }
        }
        closedir(d);
    }
    return filename;
}

void listFiles(int socket){
    char* msg = malloc(sizeof(char) * MAX_LENGTH);
    strcat(msg, "-------------List of files-------------\n");
    DIR *d = opendir("./serverFiles");
    struct dirent *dir;
    int cpt=1;
    if (d)
    {
        char* filename = malloc(sizeof(char) * MAX_LENGTH);
        while ((dir = readdir(d)) != NULL){

            if(dir->d_type==8){
                sprintf(filename, "%s : %d",dir->d_name, cpt);
                cpt++;
            }
            strcat(msg, filename);
            strcat(msg, " \n");
        }
        closedir(d);
    }
    strcat(msg,"----------------------\n");
    strcat(msg,"Utilisez la commande /dlfile <fileNb> pour envoyer un fichier au serveur\n");
    strcat(msg,"\n");
    send(socket,msg,MAX_LENGTH,0);
}

void send_file(int sockfd){
    printf("Envoi du code de début au client \n");
    send(sockfd, "200StartFileThreadReceive", MAX_LENGTH, 0);
    char* filename = mostRecentFile;
    printf("%s\n", filename);
    struct sockaddr_in aF;
    socklen_t lgF = sizeof(struct sockaddr_in) ;// Means for the client to connect to the file send service on port 4000
    int dSC = accept(dSocketFile, (struct sockaddr*)&aF, &lgF); // Accept the connection
    printf("Connection acceptée pour envoi\n");
    int tailleF=(strlen(filename)+1)*sizeof(char);
    printf("\ntailleF : %d",tailleF);
    send(dSC, &tailleF, sizeof(int), 0);
    send(dSC, filename, strlen(filename), 0);
    printf("Opening file\n");
    char* folder="serverFiles/";
    char path[strlen(filename)+strlen(folder)];
    sprintf(path,"serverFiles/%s",filename);
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) {
        printf("Erreur lors de la lecture du fichier \n Veuillez réessayer.");
        return;
    }

    printf("Sending file size \n");
    fseek(fp,0,SEEK_END);
    long file_size= ftell(fp);
    rewind(fp);
    send(dSC,&file_size,sizeof(long),0);
    printf("File size sent\n");
    printf("Sending file \n");
    char buffer[SIZE];
    int cpt;
    for(int i=0;i<file_size;i+=SIZE){
        if(i+SIZE<file_size){
            cpt=SIZE;
        }
        else{
            cpt=file_size-i;
        }
        fread(buffer,cpt,1,fp);
        send(dSC,buffer,sizeof(buffer),0);

        bzero(buffer,SIZE);
    }
    printf("File sent\n");
    fclose(fp);
    shutdown(dSC,2);
    nbFileThread--;
    pthread_exit(NULL);
}

void write_file(int sockfd){
    printf("Envoi du code de début au client\n");
    send(sockfd, "200StartFileThreadSend", MAX_LENGTH, 0);
    struct sockaddr_in aF ;
    socklen_t lgF = sizeof(struct sockaddr_in) ;// Means for the client to connect to the file send service on port 4000
    int dSC = accept(dSocketFile, (struct sockaddr*)&aF, &lgF); // Accept the connection
    printf("Connection acceptée pour reception\n");

    int taille;
    recv(dSC,&taille,sizeof(int),0);
    if(taille<0){
        pthread_exit(NULL);
    }
    printf("Taille du nom du fichier : %d\n",taille);
    printf("Reception du nom du fichier\n");
    char* fileName = (char *) malloc(taille * sizeof (char));
    recv(dSC, fileName, MAX_LENGTH, 0);
    printf("Nom du fichier : %s\n", fileName);


    long file_size;
    recv(dSC, &file_size, sizeof(long), 0);
    printf("Taille du fichier : %ld\n", file_size);
    char* folder="serverFiles/";
    char path[strlen(fileName)+strlen(folder)];
    sprintf(path,"serverFiles/%s",fileName);
    FILE *fp;
    printf("Réception du fichier\n");
    fp = fopen(path, "w+");

    int cpt;
    char buffer[SIZE];
    for(int i=0;i<file_size;i+=SIZE){
        if(i+SIZE<file_size){
            cpt=SIZE;
        }
        else{
            cpt=file_size-i;
        }
        recv(dSC, buffer, cpt, 0);
        fwrite(buffer, cpt,1, fp);
        bzero(buffer,SIZE);
    }
    fclose(fp);
    printf("File received\n");
    send(sockfd, "200FileReceived", MAX_LENGTH, 0);
    shutdown(dSC, 2);
    nbFileThread--;
    printf("Socket fichier dors \n");
    pthread_exit(NULL);
}

void redirection(char* msg, int socket) {
    int mp;
    int man;
    int list;
    int dc;
    int file;
    int listfile;
    int dlfile;
    int listcanal;
    regex_t preg;
    const char *mp_regex = "^/mp";
    const char *man_regex = "^/man";
    const char *dc_regex = "^/dc";
    const char *list_regex = "^/list";
    const char *file_regex = "^/file";
    const char *listfile_regex = "^/dllist";
    const char *listcanal_regex = "^/listcanal";
    const char *dlfile_regex = "^/dlfile";

    // Commande mp
    mp = regcomp (&preg, mp_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (mp == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            privateMessage(msg, socket, listeClient);
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
            disconnectClient(socket, listeClient);
        }
    }

    // Commande list
    list = regcomp (&preg, list_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if(list == 0){
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if(match == 0){
            envoyerListClients(socket, listeClient);
        }
    }

    // Commande file
    file = regcomp (&preg, file_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if(file == 0){
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if(match == 0){
            pthread_create(&fileThread[nbFileThread], NULL, write_file, socket);
            nbFileThread++;
        }
    }

    // Commande listfile
    listfile = regcomp (&preg, listfile_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if(listfile == 0){
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if(match == 0){
            listFiles(socket);
        }
    }

    // Commande listcanal
    listcanal = regcomp (&preg, listcanal_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (listcanal == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            //envoyerListCanaux(socket);
        }
    }

    // Commande dlfile
    dlfile = regcomp (&preg, dlfile_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (listcanal == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            char* fileName = strtok(msg, " ");
            fileName = strtok(NULL, " ");
            int fileNumber = atoi(fileName);
            mostRecentFile = getfilename(fileNumber);
            pthread_create(&fileThread[nbFileThread], NULL, send_file, socket);
            nbFileThread++;
        }
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
                envoyerMessage(msg_final, client->dSC);
            } else if(msg[0] != '\n'){ // It's a command
                redirection(msg, client->dSC);
            }
        }

    }

}

int main(int argc, char** argv){
    listeClient = creerListe();
    mostRecentFile = (char*) malloc(MAX_LENGTH);
    printf("Début programme\n");

    int dS = socket(PF_INET, SOCK_STREAM, 0);
    printf("Socket Créé\n");

    struct sockaddr_in ad;
    ad.sin_family = AF_INET;
    ad.sin_addr.s_addr = INADDR_ANY ;
    ad.sin_port = htons(PORT);
    bind(dS, (struct sockaddr*)&ad, sizeof(ad)) ;
    printf("Socket Nommé\n");

    listen(dS, 7);
    printf("Mode écoute\n");

    struct sockaddr_in aC ;
    socklen_t lg = sizeof(struct sockaddr_in) ;

    pthread_t thread[MAX_CLIENTS];
    sem_init(&semaphore, 0, 1);

    // Création socket fichier
    dSocketFile = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in adF;
    adF.sin_family = AF_INET;
    adF.sin_addr.s_addr = INADDR_ANY ;
    adF.sin_port = htons(4000); // Port of the file send service
    bind(dSocketFile, (struct sockaddr*)&adF, sizeof(adF));
    listen(dSocketFile, 7);


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