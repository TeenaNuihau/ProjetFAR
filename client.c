#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "color.h"
#include <regex.h>
#include <dirent.h>

#define MAX_LENGTH 1500 //Max length of string
#define SIZE 1024 // Taille du buffer de réception
#define PORT 3001 //Port of the server


char pseudo[100]; // Pseudo of the client
char* ipServer; // IP of the server
static volatile int keepRunning = 1; // 0 = stop, 1 = run
int dS;  // Socket of the client
pthread_t thread[4]; // 0 for receive and 1 for send and 2 for file send thread 3 for file receive thread
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int color; // Color of the client (0 = blanc, 1 = bleu, 2 = vert, 3 = orange, 4 = cyan, 5 = rouge)

char* mostRecentFileName;


void turnOff(int signal){
    keepRunning = 0;
    send(dS,"/dc", MAX_LENGTH , 0);
    exit(1);
}

char* getfilename(int n) {
    char* filename = malloc(sizeof(char) * MAX_LENGTH);
    DIR *d = opendir("./clientFiles");
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

int listfiles(int c){
    if (c == 1) {
        printf("\n");
        printcyan("----------------------\n");
    }
    DIR *d = opendir("./clientFiles");
    struct dirent *dir;
    int cpt=1;
    if (d)
    {
        while ((dir = readdir(d)) != NULL){

            if(dir->d_type==8){
                if (c == 1) {
                    printcyanfile(dir->d_name,cpt);
                }
                cpt++;
            }
            printf("\n");
        }
        closedir(d);
    }
    if (c == 1) {
        printcyan("----------------------\n");
        printcyan("Utilisez la commande /file <fileNb> pour envoyer un fichier au serveur\n");
    }
    printf("\n");
    return cpt;
}

// returns 0 if the message is a file send command, 1 otherwise
int file_command(char* msg){
    int file;
    regex_t preg;
    const char *file_regex = "^/file";
    // Commande file
    file = regcomp (&preg, file_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    int match = 1;
    if(file == 0){

        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

    }
    return match;
}

int filelist_command(char* msg){
    int file;
    regex_t preg;
    const char *file_regex = "^/listfile";
    // Commande file
    file = regcomp (&preg, file_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    int match = 1;
    if(file == 0){

        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

    }
    return match;
}

int dlfile_command(char* msg){
    int file;
    regex_t preg;
    const char *file_regex = "^/dlfile";
    // Commande file
    file = regcomp (&preg, file_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    int match = 1;
    if(file == 0){

        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

    }
    return match;
}
// returns 1 if the message is a file send response code, 0 otherwise
int file_responseSendCode(char* msg){
    int file;
    regex_t preg;
    const char *file_regex = "^200StartFileThreadSend";
    // Commande file
    file = regcomp (&preg, file_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    int match = 1;
    if(file == 0){

        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

    }
    return match;
}

// returns 1 if the message is a file recieve response code, 0 otherwise
int file_responseRecieveCode(char* msg){
    int file;
    regex_t preg;
    const char *file_regex = "^200StartFileThreadReceive";
    // Commande file
    file = regcomp (&preg, file_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    int match = 1;
    if(file == 0){

        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

    }
    return match;
}
// returns 1 if the message is a file  success send response code, 0 otherwise
int file_responseSuccessCode(char* msg){
    int file;
    regex_t preg;
    const char *file_regex = "^200FileReceived";
    // Commande file
    file = regcomp (&preg, file_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    int match = 1;
    if(file == 0){

        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

    }
    return match;
}

int color_command(char* msg){
    int color;
    regex_t preg;
    const char *color_regex = "^/color";
    // Commande color
    color = regcomp (&preg, color_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    int match = 1;
    if(color == 0){

        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

    }
    return match;
}

// Function to download files from server
void* receiveFile_command() {
    // Connexion to the new socket of the server file send service
    int sockfd_file;
    struct sockaddr_in serv_addr;
    sockfd_file = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_file < 0) {
        printf("ERROR opening socket");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(4000);
    inet_pton(AF_INET,ipServer, &serv_addr.sin_addr);
    if (connect(sockfd_file, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("ERROR connecting");
        exit(1);
    }
    printf("Connected to the file receive service\n");

    // Nom fichier
    int taille;
    recv(sockfd_file,&taille,sizeof(int),0);
    if(taille<0){
        pthread_exit(NULL);
    }
    printf("Taille du nom du fichier : %d\n",taille);
    printf("Reception du nom du fichier\n");
    char* fileName = (char *) malloc(taille * sizeof (char));
    recv(sockfd_file, fileName, taille, 0);
    printf("Nom du fichier : %s\n", fileName);

    // Fichier
    long file_size;
    recv(sockfd_file, &file_size, sizeof(long), 0);
    printf("Taille du fichier : %ld\n", file_size);
    char* folder="downloads/";
    char path[strlen(fileName)+strlen(folder)];
    sprintf(path,"downloads/%s",fileName);
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
        recv(sockfd_file, buffer, cpt, 0);
        fwrite(buffer, cpt,1, fp);
        bzero(buffer,SIZE);
    }
    fclose(fp);
    printf("File received\n");
    shutdown(sockfd_file, 2);
    pthread_exit(NULL);
}

// Fonction to send a file
void* sendFile_command() {
    // Connexion to the new socket of the server file send service
    int sockfd_file;
    struct sockaddr_in serv_addr;
    sockfd_file = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_file < 0) {
        printf("ERROR opening socket");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(4000);
    inet_pton(AF_INET,ipServer, &serv_addr.sin_addr);
    if (connect(sockfd_file, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("ERROR connecting");
        exit(1);
    }
    printf("Connected to the file send service\n");
    // Send the file name
    printf("mostRecentfilename : %s", mostRecentFileName);
;
    int tailleF=(strlen(mostRecentFileName))*sizeof(char);
    printf("\ntailleF : %d",tailleF);
    send(sockfd_file, &tailleF, sizeof(int), 0);
    send(sockfd_file, mostRecentFileName, strlen(mostRecentFileName), 0);
    printf("Opening file\n");
    char* folder="clientFiles/";
    char path[strlen(mostRecentFileName)+strlen(folder)];
    sprintf(path,"clientFiles/%s",mostRecentFileName);
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) {
        printf("Erreur lors de la lecture du fichier \n Veuillez réessayer.");
        return 0;
    }

    printf("Sending file size \n");
    fseek(fp,0,SEEK_END);
    long file_size= ftell(fp);
    rewind(fp);
    send(sockfd_file,&file_size,sizeof(long),0);
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
        send(sockfd_file,buffer,sizeof(buffer),0);

        bzero(buffer,SIZE);
    }
    printf("File sent\n");
    fclose(fp);
    shutdown(sockfd_file,2);
    pthread_exit(NULL);
}




void recevoir(int* s){
    char* m = (char *) malloc(MAX_LENGTH);
    while(keepRunning){
        bzero(m,MAX_LENGTH);
        recv(*s, m, MAX_LENGTH, 0);
        // Stands for the response of the server in case of starting sending a file
        if(file_responseSendCode(m) == 0){
            pthread_create(&thread[2], NULL, sendFile_command, NULL);

        } // Stands for the response of the server of the conclusion of the file send service
        else if (file_responseSuccessCode(m)==0){
            printf("File received successfully by the server\n");

        } else if (file_responseRecieveCode(m)==0){
            printf("File sent sequence start\n");
            pthread_create(&thread[4], NULL, receiveFile_command, NULL);
        }
        else {
            printf("\n%s",m);
        }
    }

    shutdown(*s,2) ;
}

void envoyer(int* s){
    char* m = (char *) malloc(MAX_LENGTH);
    char* m2 = "/dc\\0";
    while(keepRunning){
        bzero(m,MAX_LENGTH);
        switch (color) { //change the color of the text in the terminal
            case 0:
                printf("\nvous : ");
                break;
            case 1:
                printbleu("\nvous : ");
                break;
            case 2:
                printvert("\nvous : ");
                break;
            case 3:
                printorange("\nvous : ");
                break;
            case 4:
                printcyan("\nvous : ");
                break;
            case 5:
                printrouge("\nvous : ");
                break;
        }
        fgets( m, MAX_LENGTH, stdin );
        // if file command
        if (file_command(m)==0) {
            // Send the message with the name of the file
            //get the name of the file
            send(*s, m, MAX_LENGTH, 0);
            char* fileName = strtok(m, " ");
            fileName = strtok(NULL, " ");
            int fileNumber = atoi(fileName);
            mostRecentFileName = getfilename(fileNumber);
            //if filelist command
        } else if (filelist_command(m)==0){
            listfiles(1);
        }
        // if deconnexion command
        else if (strcmp(m,m2) == 0){
            printf("\nDéconnexion\n");
            turnOff(0);
        }
        else if(color_command(m) == 0){
            //get the int value of the color
            char* colorMsg = strtok(m, " ");
            colorMsg = strtok(NULL, " ");
            color = atoi(colorMsg);
        }
        // else send to server as it is
        else {
            send(*s, m, MAX_LENGTH, 0);
        }
    }
    shutdown(*s,2) ;
}


int main(int argc, char *argv[]) {
    // Check if the number of arguments is correct
    if (argc != 2) {
        printf("Usage: %s <ip> \n", argv[0]);
        exit(1);
    }
    mostRecentFileName = (char*) malloc(MAX_LENGTH);
    ipServer = argv[1];
    printf("Début programme\n");
    dS = socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket Créé\n");

    struct sockaddr_in aS;
    aS.sin_family = AF_INET;
    inet_pton(AF_INET,argv[1],&(aS.sin_addr)) ;
    aS.sin_port = htons(PORT);
    socklen_t lgA = sizeof(struct sockaddr_in) ;
    connect(dS, (struct sockaddr *) &aS, lgA) ;
    printf("Socket Connecté\n");


    printf("Entrez votre pseudo : ");
    fgets(pseudo, 100, stdin);
    send(dS, pseudo, MAX_LENGTH, 0);

    signal(SIGINT, turnOff);

    pthread_create(&thread[1], NULL,(void *)envoyer,&dS);
    pthread_create(&thread[0], NULL,(void *)recevoir,&dS);

    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);


    printf("J'ai fini \n");

}
