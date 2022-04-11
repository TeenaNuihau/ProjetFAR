#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
  aS.sin_port = htons(atoi(argv[2])) ;
  socklen_t lgA = sizeof(struct sockaddr_in) ;
  connect(dS, (struct sockaddr *) &aS, lgA) ;
  printf("Socket Connecté\n");

  char msg [100] ;
  recv(dS, msg, sizeof(msg), 0) ;
  printf("Message reçu : %s\n", msg) ;


  char * m = "Je répond" ;
  send(dS, m, strlen(m) , 0) ;
  printf("Réponse Envoyé \n");

  /*int r;
  recv(dS, &r, sizeof(int), 0) ;
  printf("Réponse reçue : %d\n", r) ;*/

  shutdown(dS,2) ;
  printf("Fin du programme");
}