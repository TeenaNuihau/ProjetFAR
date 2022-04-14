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

  // Connexion Client 1
  int dSC1 = accept(dS, (struct sockaddr*) &aC,&lg) ;
  printf("Client 1 Connecté\n");
  char* wait = "wait";
  send(dSC1, wait, MAX_LENGTH , 0);

  // Connexion Client 2
  int dSC2 = accept(dS, (struct sockaddr*) &aC,&lg) ;
  printf("Client 2 Connecté\n");
  char* sender = "send";
  send(dSC2, sender, MAX_LENGTH , 0);

  while (1) {

    char msg [MAX_LENGTH] ;
    recv(dSC2, msg, MAX_LENGTH, 0) ;
    printf("Premier Message reçu : %s \n", msg) ;
  
    send(dSC1, msg, MAX_LENGTH, 0) ;
    printf("Premier Message Envoyé\n");
    
    char rep [MAX_LENGTH] ;
    recv(dSC1, rep, MAX_LENGTH, 0) ;
    printf("Deuxieme Réponse reçu : %s\n", rep) ;

    send(dSC2, rep, MAX_LENGTH, 0) ;
    printf("Deuxieme Réponse envoyée\n");

  }



  
  shutdown(dSC1, 2) ; 
  shutdown(dSC2, 2) ; 
  shutdown(dS, 2) ;
  printf("Fin du programme \n");
}