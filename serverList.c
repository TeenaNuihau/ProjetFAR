//
// Created by Romain Mezghenna, Mathieu Dos Santos, Nuihau Teena on 7/04/2022.
//

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <regex.h>
#include "help.h"
#include <dirent.h>


#define PORT 3001 //Port of the server
#define MAX_CANALS 10 //Max number of canals


Liste *listeClient; //Liste des clients
sem_t semaphore; //Semaphore pour le mutex
pthread_t fileThread[20]; //Tableau de thread pour les fichiers
static volatile int keepRunning = 1; //Variable pour la boucle de l'écoute
int canaux[MAX_CANALS]; //Tableau des canaux
int nextCanalID = 1; //ID du prochain canal
int nextCanalIndice = 0; //Indice du prochain canal
int nbFileThread = 0; //Nombre de thread pour les fichiers
int dSocketFile; //Socket du thread fichier
char* mostRecentFile; //Nom du fichier le plus récent



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

void envoyerMessage(char* msg, int from){
    element* expedi = rechercherElementSocket(listeClient, from);
    element* tmp = listeClient->head;
    while(tmp != NULL && tmp->isConnected == 1){
        if(tmp->dSC != from && tmp->canal == expedi->canal){
            char* msg_final = (char*) malloc(MAX_LENGTH);
            sprintf(msg_final, "[Canal %d] %s",expedi->canal,msg);
            send(tmp->dSC, msg_final, MAX_LENGTH, 0);
        }
        tmp = tmp->nxt;
    }
}

void creerListeCanaux(){
    // Get all the canaux in the file listeCanaux.txt and put them in the canaux array
    FILE* fichier = fopen("serverConfig/listeCanaux.txt", "r");
    char ligne[MAX_LENGTH];
    while(fgets(ligne, MAX_LENGTH, fichier) != NULL){
        canaux[nextCanalIndice] = atoi(ligne);
        nextCanalIndice++;
        if(atoi(ligne) >= nextCanalID){
            nextCanalID = atoi(ligne) + 1;
        }
    }
    fclose(fichier);
}

void enregistrerCanaux(){
   // Save all the current canaux in the file listeCanaux.txt
    FILE* fichier = fopen("serverConfig/listeCanaux.txt", "w+");
    int i = 0;
    while(i < MAX_CANALS){
        if (canaux[i] > 0){
            fprintf(fichier, "%d\n", canaux[i]);
        }
        i++;
    }
    fclose(fichier);
}

void ajouterCanal(int socket){
    char* msg = (char*)malloc(MAX_LENGTH);
    if(nextCanalIndice < MAX_CANALS){
        canaux[nextCanalIndice] = nextCanalID;
        nextCanalIndice++;
        nextCanalID++;
        sprintf(msg, "Canal créé : %d", nextCanalID - 1);
    } else {
        sprintf(msg, "Impossible de créer de nouveau canal");
    }
    send(socket, msg, MAX_LENGTH, 0);
}

void supprimerCanal(int id){
    //Remove all the clients in the canal and push them to the default canal (0)
    element* tmp = listeClient->head;
    while(tmp != NULL){
        if(tmp->canal == id){
            tmp->canal = 0;
            send(tmp->dSC, "Vous avez été déplacé dans le canal par défaut", MAX_LENGTH, 0);
        }
        tmp = tmp->nxt;
    }
    //Remove the canal from the canaux array
    int i = 0;
    while(i < MAX_CANALS){
        if(canaux[i] == id){
            canaux[i] = 0;
            break;
        }
        i++;
    }
}

int canalExiste(int id){
    int i = 0;
    while(i < MAX_CANALS){
        if(canaux[i] == id){
            return 1;
        }
        i++;
    }
    return 0;
}

void envoyerListeCanaux(int socket){
    char* msg = (char*)malloc(MAX_LENGTH);
    sprintf(msg, "Liste des canaux : \n");
    int i = 0;
    while(i < MAX_CANALS){
        char* canal = (char*)malloc(MAX_LENGTH);
        if(canaux[i] > 0){
            sprintf(canal, "Canal %d\n", canaux[i]);
            strcat(msg, canal);
        }
        i++;
    }
    send(socket, msg, MAX_LENGTH, 0);
}

void connecterToCanal(int socketClient, int idCanal){
    element* expedi = rechercherElementSocket(listeClient, socketClient);
    char* msg = (char*)malloc(MAX_LENGTH);
    if(expedi != NULL && canalExiste(idCanal)){
        expedi->canal = idCanal;
        sprintf(msg, "Connecté au canal %d", idCanal);
    } else {
        sprintf(msg, "Impossible de se connecter au canal %d", idCanal);
    }
    send(socketClient, msg, MAX_LENGTH, 0);
}

void deconnecterFromCanal(int socketClient){
    element* expedi = rechercherElementSocket(listeClient, socketClient);
    if(expedi != NULL){
        expedi->canal = 0;
        char* msg = (char*)malloc(MAX_LENGTH);
        sprintf(msg, "Déconnecté du canal \n Vous êtes maintenant dans le canal général");
        send(socketClient, msg, MAX_LENGTH, 0);
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
    int tailleF=(strlen(filename))*sizeof(char);
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

void kickClient(int from, char* pseudo){
    element* banneur = rechercherElementSocket(listeClient, from);
    element* banni = rechercherElementPseudo(listeClient, pseudo);
    if(banni != NULL){
        char* msg = (char*)malloc(MAX_LENGTH);
        sprintf(msg, "Vous avez été expulsé de la messagerie par %s", banneur->pseudo);
        char* dc = (char*)malloc(MAX_LENGTH);
        sprintf(dc, "/dc");
        send(banni->dSC, msg, MAX_LENGTH, 0);
        send(banni->dSC, dc, MAX_LENGTH, 0);
    }
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
    int ccanal;
    int dcanal;
    int cgcanal;
    int dccanal;
    int canals;
    int kick;
    regex_t preg;
    const char *mp_regex = "^/mp";
    const char *man_regex = "^/man";
    const char *dc_regex = "^/dc";
    const char *list_regex = "^/list";
    const char *file_regex = "^/file";
    const char *listfile_regex = "^/dllist";
    const char *listcanal_regex = "^/canallist";
    const char *dlfile_regex = "^/dlfile";
    const char *ccanal_regex = "^/canalcr";
    const char *dcanal_regex = "^/canaldl";
    const char *cgcanal_regex = "^/canalcg";
    const char *dccanal_regex = "^/canaldc";
    const char *canals_regex = "^/canals";
    const char *kick_regex = "^/kick";

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
            //get the canal of the client
            element* client = rechercherElementSocket(listeClient, socket);
            envoyerListClientsCanal(socket, client->canal,listeClient);
        }
    }

    // Commande ccanal
    ccanal = regcomp (&preg, ccanal_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (ccanal == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            //create channel
            ajouterCanal(socket);
        }
    }

    // Commande dcanal
    dcanal = regcomp (&preg, dcanal_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (dcanal == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            //delete channel and redirect client to channel 0
            element* client = rechercherElementSocket(listeClient, socket);
            int idCanal = client->canal;
            char* rep = (char*) malloc(MAX_LENGTH);
            if (idCanal == 0){
                strcpy(rep, "Vous ne pouvez pas supprimer le canal 0");
            }
            else{
                supprimerCanal(idCanal);
                strcpy(rep, "Vous avez bien supprimer le canal");
            }
            send(socket, rep, MAX_LENGTH, 0);
        }
    }

    // Commande cgcanal
    cgcanal = regcomp (&preg, cgcanal_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (cgcanal == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            //delete channel and redirect client to channel 0
            element* client = rechercherElementSocket(listeClient, socket);
            char* idcanal = strtok(msg, " ");
            idcanal = strtok(NULL, " ");
            int numcanal = atoi(idcanal);
            connecterToCanal(socket, numcanal);
        }
    }

    // Commande dccanal
    dccanal = regcomp (&preg, dccanal_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (dccanal == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            //delete channel and redirect client to channel 0
            element* client = rechercherElementSocket(listeClient, socket);
            connecterToCanal(socket, 0);
        }
    }

    // Commande canals
    canals = regcomp (&preg, canals_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (canals == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            envoyerListeCanaux(socket);

        }
    }

    // Commande dlfile
    dlfile = regcomp (&preg, dlfile_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (dlfile == 0) {
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
    // Commande kick
    kick = regcomp (&preg, kick_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (kick == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            char* pseudoR = strtok(msg, " ");
            pseudoR = strtok(NULL, " ");
            size_t taille = strlen(pseudoR)-1;
            char pseudo[taille];
            strncpy(pseudo,pseudoR,(size_t)(taille));
            pseudo[taille]='\0';
            printf("pseudo a kick : %s\n",pseudo);
            kickClient(socket, pseudo);
        }
    }
}

void traitementClient(element* client){
    char msg[MAX_LENGTH];
    printf("Client %d connected\n", client->dSC);
    commandManual(client->dSC); //affiche les commandes disponibles
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

void turnOff(int signal){
    //disconnect all clients
    element* client = listeClient->head;
    while(client != NULL){
        send(client->dSC, "Server is going down\n", MAX_LENGTH, 0);
        disconnectClient(client->dSC,listeClient);
        client = client->nxt;
    }
    keepRunning = 0;
    enregistrerCanaux();
    exit(1);
}

int main(int argc, char** argv){
    listeClient = creerListe();
    mostRecentFile = (char*) malloc(MAX_LENGTH);
    creerListeCanaux();
    signal(SIGINT, turnOff);
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


    while(listeClient->nbElements < MAX_CLIENTS && keepRunning){
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
    shutdown(dS, SHUT_RDWR);
    return 0;

}