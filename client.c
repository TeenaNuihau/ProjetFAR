#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <regex.h>

#define MAX_LENGTH 100
#define PORT 3001
#define SIZE 1024

char pseudo[100];


void send_file(FILE *fp, int sockfd){
    int n;
    char data[SIZE] = {0};

    while(fgets(data, SIZE, fp) != NULL) {
        bzero(data, SIZE);
    }
}

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


void sendFile_command(int* sockfd, char* filename) {
    FILE* fp = fopen("msg_test.txt", "rb");
    if (fp == NULL) {
        printf("[-]Error in reading file.");
        return;
    }
    printf("ici\n");
    send_file(fp, *sockfd);
    printf("[+]File data sent successfully.\n");
}

void recevoir(int* s){
  char* m = (char *) malloc(MAX_LENGTH);
  while(1){
    recv(*s, m, MAX_LENGTH, 0) ;
    printf("\n%s",m) ;
    //free( m );
  }
  shutdown(*s,2) ;
}

void envoyer(int* s){
  
  char* m = (char *) malloc(MAX_LENGTH);
  while(1){
    //printf("%s : \n",pseudo);
    printf("\nvous : ");
    fgets( m, MAX_LENGTH, stdin );
    if (file_command(m)==0) {
        char delim[] = " ";
        char *p = strtok(m, delim);
        p = strtok(NULL, delim);
        char* filename = p;
        printf("filename = %s\n",p);
        sendFile_command(s,filename);
    }
    //char* message = strcat(p,m);
    send(*s,m, MAX_LENGTH , 0);
    //free( m );
  }
  shutdown(*s,2) ;
}


int main(int argc, char *argv[]) {

  printf("Début programme\n");
  int dS = socket(PF_INET, SOCK_STREAM, 0);
  printf("Socket Créé\n");

  struct sockaddr_in aS;
  aS.sin_family = AF_INET;
  inet_pton(AF_INET,argv[1],&(aS.sin_addr)) ;
  // aS.sin_port = htons(atoi(argv[2]));
  aS.sin_port = htons(PORT);
  socklen_t lgA = sizeof(struct sockaddr_in) ;
  connect(dS, (struct sockaddr *) &aS, lgA) ;
  printf("Socket Connecté\n");

  
  printf("Entrez votre pseudo : ");
  fgets(pseudo, 100, stdin);
  send(dS, pseudo, MAX_LENGTH, 0);


  pthread_t thread[2];
  int tReception;
  int tEnvoi;


  tEnvoi=pthread_create(&thread[1], NULL,(void *)envoyer,&dS);
  tReception=pthread_create(&thread[0], NULL,(void *)recevoir,&dS);

  pthread_join(thread[0], NULL);
  pthread_join(thread[1], NULL);


  printf("J'ai fini \n");

}
