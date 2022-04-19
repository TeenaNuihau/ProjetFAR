#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_LENGTH 100
#define MAX_CLIENTS 50

void communiquerWithCli(int socketCli , int i , int descClients[]){
  while(1){
    char msg [MAX_LENGTH] ;
    recv(socketCli, msg, MAX_LENGTH, 0) ;
    printf("Premier Message reçu : %s \n", msg) ;
    for(int j=0;j<=i;j++){
      if(socketCli!=descClients[j]){
        send(descClients[j], msg, MAX_LENGTH,0);
      }
    }
    printf("Message Envoyé\n");
  }
  shutdown(socketCli,2); // MAX_CLIENTS
}

typedef struct {
  int dSC;
  int i;
  int desc[MAX_CLIENTS];
} para;


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
  
  pthread_t thread[MAX_CLIENTS]; 
  int desc[MAX_CLIENTS];
  long i =0;

  while (1 && i<=MAX_CLIENTS) {
    // Connexion Client 
    int dSC = accept(dS, (struct sockaddr*) &aC,&lg) ;
    printf("Client Connecté\n");
    desc[i]=dSC;
    
    //char* wait = "wait";
    //send(dSC1, wait, MAX_LENGTH , 0);
    
    para p;
    p.dSC=dSC;
    p.i=i;
    for(int l=0;l<MAX_CLIENTS;l++)
      p.desc[l]=desc[l];

    if(i>1){
      pthread_create(&thread[i], NULL,(void *)communiquerWithCli,&p); 
      pthread_join(thread[i], NULL); 
      /*
      char msg [MAX_LENGTH] ;
      recv(dSC2, msg, MAX_LENGTH, 0) ;
      printf("Premier Message reçu : %s \n", msg) ;
    
      send(dSC1, msg, MAX_LENGTH, 0) ;
      printf("Premier Message Envoyé\n");
      
      char rep [MAX_LENGTH] ;
      recv(dSC1, i++;rep, MAX_LENGTH, 0) ;
      printf("Deuxieme Réponse reçu : %s\n", rep) ;

      send(dSC2, rep, MAX_LENGTH, 0) ;
      printf("Deuxieme Réponse envoyée\n");
      */
    }
    i++;
    printf("%ld",i);
  }
  
  /*shutdown(dSC1, 2) ; 
  shutdown(dSC2, 2) ; 
  shutdown(dS, 2) ;*/
  printf("Fin du programme \n");
}