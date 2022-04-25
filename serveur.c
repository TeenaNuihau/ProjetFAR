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
#define PORT 3001

int desc[MAX_CLIENTS];

long i=-1;


void communiquerWithCli(){
  //int nbcli=(int)( sizeof(*(p->desc)) / sizeof(*(p->desc[0]))); 
  while(1){
    printf("Nombre de clients : %ld\n" ,i);
    if(i>0){
      printf("Je tente de communiquer\n");
      while(1){
        printf("Je boucle\n");
        char msg [MAX_LENGTH] ;
        for (int k=0;k<=i;k++) {
          printf("desc[%d] = %d \n",k, desc[k]);
          if (recv(desc[k], msg, MAX_LENGTH, 0)!=-1) {
            printf("Premier Message reçu : %s \n", msg) ;
            for(int j=0;j<=i;j++){
              if(desc[k]!=desc[j]){
                printf("Je parle avec le client %d\n",j);
                send(desc[j], msg, MAX_LENGTH,0);
              }
            }
          printf("Message Envoyé\n");
          }
          printf("k = %d \n", k);
        }
      }
      i--;
      shutdown(desc[i],2); // MAX_CLIENTS
    }
  }
}

void metAjourTableau(int socket){
  i++;
  desc[i]=socket;
  printf("Je met à jour : %d \n",desc[i]);
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
  
  int cpt=0;
  
  int t=pthread_create(&thread[0], NULL,(void *)communiquerWithCli,NULL); 
  
  while (cpt<=MAX_CLIENTS) {
    // Connexion Client 
    int dSC = accept(dS, (struct sockaddr*) &aC,&lg) ;
    printf("Client %d Connecté\n", dSC);
    metAjourTableau(dSC);
    cpt++;
    printf("i = %d \n",cpt);
  }
  

  printf("Fin du programme \n");
}