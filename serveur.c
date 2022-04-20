#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_LENGTH 100
#define MAX_CLIENTS 50


typedef struct {
  int* dSC;
  int* desc[MAX_CLIENTS];
} para;


void communiquerWithCli(para* p){
  int nbcli=(int)( sizeof(*(p->desc)) / sizeof(*(p->desc[0]))); 
  printf("Nombre de clients : %d" ,nbcli);
  printf("Je tente de communiquer");
  while(1){
    sleep(1);
    printf("Je boucle");
    char msg [MAX_LENGTH] ;
    recv(*(p->dSC), msg, MAX_LENGTH, 0) ;
    printf("Premier Message reçu : %s \n", msg) ;
    for(int j=0;j<nbcli;j++){
      if(*(p->dSC)!=*(p->desc[j])){
        printf("Je parle avec le client %d",j);
        send(*(p->desc[j]), msg, MAX_LENGTH,0);
      }
    }
    printf("Message Envoyé\n");
  }
  shutdown(*(p->dSC),2); // MAX_CLIENTS
}


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
  long i=0;

  while (1 && i<=MAX_CLIENTS) {
    // Connexion Client 
    int dSC = accept(dS, (struct sockaddr*) &aC,&lg) ;
    printf("Client Connecté\n");
    desc[i]=dSC;

    para p;
    p.dSC=&dSC;
    for(int l=0;l<=i;l++)
      p.desc[l]=&desc[l];
      
    int t;
    t=pthread_create(&thread[i], NULL,(void *)communiquerWithCli,&p); 

    
    i++;
    printf("%ld",i);
  }
  

  printf("Fin du programme \n");
}