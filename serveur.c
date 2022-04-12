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


  struct sockaddr_in ad;
  ad.sin_family = AF_INET;
  ad.sin_addr.s_addr = INADDR_ANY ;
  // ad.sin_port = htons(atoi(argv[1])) ;
  ad.sin_port = htons(3000);
  bind(dS, (struct sockaddr*)&ad, sizeof(ad)) ;
  printf("Socket Nommé\n");

  listen(dS, 7) ;
  printf("Mode écoute\n");

  struct sockaddr_in aC ;
  socklen_t lg = sizeof(struct sockaddr_in) ;
  int dSC1 = accept(dS, (struct sockaddr*) &aC,&lg) ;
  printf("Client 1 Connecté\n");

  int dSC2 = accept(dS, (struct sockaddr*) &aC,&lg) ;
  printf("Client 2 Connecté\n");

  char msg [100] ;
  recv(dSC1, msg, 100, 0) ;
  printf("Message reçu : %s \n", msg) ;
  
  int r = 10 ;
 
  send(dSC2, msg, 100, 0) ;
  printf("Message Envoyé\n");
  
  char rep [100] ;
  recv(dSC2, rep, 100, 0) ;
  printf("Réponse reçu : %s\n", rep) ;

  send(dSC1, rep, 100, 0) ;
  printf("Réponse envoyée\n");

  
  shutdown(dSC1, 2) ; 
  shutdown(dSC2, 2) ; 
  shutdown(dS, 2) ;
  printf("Fin du programme");
}