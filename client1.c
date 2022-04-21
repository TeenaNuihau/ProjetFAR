#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_LENGTH 100
#define PORT 3002


void recevoir(int* s){
  char* m = (char *) malloc(MAX_LENGTH);
  while(1){
    recv(*s, m, MAX_LENGTH, 0) ;
    printf("Message reçu : %s\n",m) ;
    //free( m );
  }
  shutdown(*s,2) ;
}

void envoyer(int* s){
  char* m = (char *) malloc(MAX_LENGTH);
  while(1){
    printf("Entrez le premier message : \n");
    fgets( m, MAX_LENGTH, stdin ); 
    send(*s, m, MAX_LENGTH , 0);
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

  pthread_t thread[2];
  int tReception;
  int tEnvoi;
  tEnvoi=pthread_create(&thread[1], NULL,(void *)envoyer,&dS); 
  tReception=pthread_create(&thread[0], NULL,(void *)recevoir,&dS); 
  
  pthread_join(thread[0], NULL);
  pthread_join(thread[1], NULL);
  printf("J'ai commencé l'émission / reception \n");

}
