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
#define PORT 3002


typedef struct {
  int* dSC;
  int* desc[MAX_CLIENTS];
} para;

long i=0;


void communiquerWithCli(para* p){
  i++;
  //int nbcli=(int)( sizeof(*(p->desc)) / sizeof(*(p->desc[0]))); 
  printf("Nombre de clients : %ld\n" ,i);
  printf("Je tente de communiquer\n");
  while(1){
    
    printf("Je boucle\n");
    char msg [MAX_LENGTH] ;
    for (int k=0;k<i;k++) {
      if (recv(*(p->desc[k]), msg, MAX_LENGTH, 0)!=-1) {
        printf("Premier Message reçu : %s \n", msg) ;
        for(int j=0;j<i;j++){
          if(*(p->desc[k])!=*(p->desc[j])){
            printf("Je parle avec le client %d\n",j);
            send(*(p->desc[j]), msg, MAX_LENGTH,0);
          }
        }
        printf("Message Envoyé\n");
      }
    printf("k = %d \n", k);
    }
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
  ad.sin_port = htons(PORT);
  bind(dS, (struct sockaddr*)&ad, sizeof(ad)) ;
  printf("Socket Nommé\n");

  listen(dS, 7) ;
  printf("Mode écoute\n");

  struct sockaddr_in aC ;
  socklen_t lg = sizeof(struct sockaddr_in) ;
  
  pthread_t thread[MAX_CLIENTS]; 
  int desc[MAX_CLIENTS];
  long cpt=0;

  while (1 && i<=MAX_CLIENTS) {
    // Connexion Client 
    int dSC = accept(dS, (struct sockaddr*) &aC,&lg) ;
    printf("Client Connecté\n");
    desc[cpt]=dSC;

    para p;
    p.dSC=&dSC;
    for(int l=0;l<=cpt;l++)
      p.desc[l]=&desc[l];
      
    int t;
    t=pthread_create(&thread[i], NULL,(void *)communiquerWithCli,&p); 

    
    cpt++;
    printf("i = %ld \n",i);
  }
  

  printf("Fin du programme \n");
}