#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

#define MAX_LENGTH 100
#define MAX_CLIENTS 50
#define PORT 3001

int desc[MAX_CLIENTS];


long i=-1;
sem_t semaphore;


void communiquerWithCli(){
  //i++;
  int socket=i;
  printf("Com avec cli %d\n",socket);
  char msg [MAX_LENGTH] ;
  while(1){
    if (recv(desc[socket], msg, MAX_LENGTH, 0)!=-1) {
      printf("%s",msg) ;
      for(int j=0;j<=i;j++){
        if(desc[socket]!=desc[j]){
          printf("Je parle avec le client %d\n",j);
          send(desc[j], msg, MAX_LENGTH,0);
        }
      }
    }
  }
  i--;
  shutdown(desc[socket],2); // MAX_CLIENTS
}

  


void metAjourTableau(int socket){
  sem_wait(&semaphore);
  i++;
  desc[i]=socket;
  printf("Je met à jour : %d \n",desc[i]);
  sem_post(&semaphore);
  printf("Je LIBERE : %ld \n",i);
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
  
  //int cpt=0;   
  sem_init(&semaphore, 0, 1);
  
  while (i<=MAX_CLIENTS) {
    // Connexion Client 
    int dSC = accept(dS, (struct sockaddr*) &aC,&lg) ;
    printf("Client %d Connecté\n", dSC);


    // Ajout du client
    metAjourTableau(dSC);

    // Communication entre clients
    int t=pthread_create(&thread[0], NULL,(void *)communiquerWithCli,NULL);
    printf("i = %ld \n",i);
  }
  

  printf("Fin du programme \n");
}