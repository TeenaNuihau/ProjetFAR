#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LENGTH 100

int main(int argc, char *argv[]) {

  printf("Début programme\n");
  int dS = socket(PF_INET, SOCK_STREAM, 0);
  printf("Socket Créé\n");

  /*struct sockaddr_in ad;
  ad.sin_family = AF_INET;
  ad.sin_addr.s_addr = INADDR_ANY ;
  ad.sin_port = htons(atoi(argv[1])) ;
  bind(dS, (struct sockaddr*)&ad, sizeof(ad)) ;
  printf("Socket Nommé\n");

  listen(dS, 7) ;
  printf("Mode écoute\n");*/

  struct sockaddr_in aS;
  aS.sin_family = AF_INET;
  inet_pton(AF_INET,argv[1],&(aS.sin_addr)) ;
  // aS.sin_port = htons(atoi(argv[2]));
  aS.sin_port = htons(3000);
  socklen_t lgA = sizeof(struct sockaddr_in) ;
  connect(dS, (struct sockaddr *) &aS, lgA) ;
  printf("Socket Connecté\n");

  while(1) {

    char msg [100] ;
    recv(dS, msg, 100, 0) ;
    printf("Message reçu : %s \n", msg) ;

    
    // char rep [100];
    // printf("Entrez une réponse : \n");
    // int n = fgets(rep,100,stdin);
    // rep[n] = '\0';

    char * m = (char *) malloc( MAX_LENGTH );
    printf("Entrez un message : ");
    fgets( m, MAX_LENGTH, stdin );  

    send(dS, m, MAX_LENGTH , 0) ;

    free( m );

    printf("Réponse Envoyé \n");

  }

  

  /*int r;
  recv(dS, &r, sizeof(int), 0) ;
  printf("Réponse reçue : %d\n", r) ;*/

  shutdown(dS,2) ;
  printf("Fin du programme \n");
}