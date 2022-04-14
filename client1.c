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

  struct sockaddr_in aS;
  aS.sin_family = AF_INET;
  inet_pton(AF_INET,argv[1],&(aS.sin_addr)) ;
  // aS.sin_port = htons(atoi(argv[2]));
  aS.sin_port = htons(3000);
  socklen_t lgA = sizeof(struct sockaddr_in) ;
  connect(dS, (struct sockaddr *) &aS, lgA) ;
  printf("Socket Connecté\n");


  // char m [100] ;
  // printf("Entrez un message : \n");
  // int n = fgets(m,100,stdin);
  // m[n] = '\0';

  // Réponse serveur (waiter or sender)
  char role [MAX_LENGTH] ;
  recv(dS, role, MAX_LENGTH, 0);
  puts(role);

  int result = strcmp(role, "send");
  char * m = (char *) malloc( MAX_LENGTH );
  if (result==0) {

    while (1) {

      printf("Entrez le premier message : ");
      fgets( m, MAX_LENGTH, stdin );  

      send(dS, m, MAX_LENGTH , 0);

      free( m );
      m = (char *) realloc(m, MAX_LENGTH);
      
      printf("Premier Message Envoyé \n");

      char rep [MAX_LENGTH];
      recv(dS, rep, MAX_LENGTH, 0) ;
      printf("Deuxième Réponse reçue : ") ;
      puts(rep);

    }

  } else {

    while(1) {

    char msg [100] ;
    recv(dS, msg, 100, 0) ;
    printf("Premier Message reçu : %s \n", msg) ;

    printf("Entrez le deuxième message : ");
    fgets( m, MAX_LENGTH, stdin );  

    send(dS, m, MAX_LENGTH , 0) ;

    free( m );
    m = (char *) realloc(m, MAX_LENGTH);

    printf("Réponse Envoyé \n");

    }

  }

  

  

  shutdown(dS,2) ;
  printf("Fin du programme \n");
}