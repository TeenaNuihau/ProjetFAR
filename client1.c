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

  struct sockaddr_in aS;
  aS.sin_family = AF_INET;
  inet_pton(AF_INET,argv[1],&(aS.sin_addr)) ;
  // aS.sin_port = htons(atoi(argv[2]));
  aS.sin_port = htons(3000);
  socklen_t lgA = sizeof(struct sockaddr_in) ;
  connect(dS, (struct sockaddr *) &aS, lgA) ;
  printf("Socket Connecté\n");


  char m [100] ;
  printf("Entrez un message : \n");
  fgets(m,100,stdin);
  send(dS, m, strlen(m) , 0);
  
  printf("Message Envoyé \n");

  char rep [100];
  recv(dS, rep, 100, 0) ;
  printf("Réponse reçue : %s\n", rep) ;

  shutdown(dS,2) ;
  printf("Fin du programme");
}