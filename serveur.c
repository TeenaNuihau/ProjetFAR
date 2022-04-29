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
#define PORT 3000

// int desc[MAX_CLIENTS];
// char* pseudos[MAX_CLIENTS];



typedef struct {
  int desc;
  char* pseudo;
} client ;

client* clients[MAX_CLIENTS];

long i=-1;
sem_t semaphore;


void communiquerWithCli(){
  //i++;
  int socket=i;
  printf("Com avec cli %d\n",socket);
  char msg [MAX_LENGTH] ;
  while(1){
    if (recv(clients[socket]->desc, msg, MAX_LENGTH, 0)!=-1) {
      
      char* msg_final = "";
      strcat(msg_final,clients[socket]->pseudo);
      printf("message final reçu : %s",msg_final) ;
      strcat(msg_final, " : ");
      strcat(msg_final, msg);
      printf("message final reçu : %s",msg_final) ;
      for(int j=0;j<=i;j++){
        if(clients[socket]->desc!=clients[j]->desc){
          send(clients[j]->desc, msg_final, MAX_LENGTH,0);
        }
      }
    }
  }
  i--;
  shutdown(clients[socket]->desc,2); // MAX_CLIENTS
}

  


void metAjourTableau(client* c){
  sem_wait(&semaphore);
  i++;
  clients[i] = c;
  sem_post(&semaphore);
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

    // Infos Client
    char* pseudoR = (char *) malloc(MAX_LENGTH);
    recv(dSC, pseudoR, MAX_LENGTH, 0);
    size_t taille = strlen(pseudoR)-1;
    char pseudo[taille];
    strncpy(pseudo,pseudoR,(size_t)(taille));
    pseudo[taille]='\0';
    printf("Client %d Connecté sous le pseudo %s\n", dSC, pseudo);

    client c;
    c.desc = dSC;
    c.pseudo = pseudo;

    // Ajout du client
    metAjourTableau(&c);

    // Communication entre clients
    int t=pthread_create(&thread[0], NULL,(void *)communiquerWithCli,NULL);
    printf("i = %ld \n",i);
  }
  

  printf("Fin du programme \n");
}