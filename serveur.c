#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <regex.h>

#define MAX_LENGTH 100
#define MAX_CLIENTS 50
#define PORT 3000

int desc[MAX_CLIENTS];
char pseudos[MAX_CLIENTS][MAX_LENGTH];

/*
typedef struct {
  int desc;
  char* pseudo;
} client ;

client* clients[MAX_CLIENTS];
*/

long i=-1;
sem_t semaphore;

int trouverPseudo(char* pseudo){
  for(int i=0;i<MAX_CLIENTS;i++)
    if(strcmp(pseudos[i],pseudo)==0)
      return i;
}

void privateMessage(char* msg, int socket) {
  char delim[] = " ";
  char *p = strtok(msg, delim);
  p = strtok(NULL, delim);
  char* desti = p;
  
  char* msg_final = (char *) malloc(MAX_LENGTH);
  strcat(msg_final, "(privé) ");
  strcat(msg_final,pseudos[socket]);
  strcat(msg_final, " : ");
  p = strtok(NULL, delim);

  while ( p != NULL ) {
    strcat(msg_final, " ");
    strcat(msg_final, p);
    p = strtok(NULL, delim);
  }
  
  int index = trouverPseudo(desti);
  send(desc[index],msg_final,MAX_LENGTH,0);
  free(msg_final);
}

void commandManual(int socket) {
  char* msg_final = "/man : display the commands list \n/mp username msg : send private message \n/dc : disconnect \n";
  send(desc[socket], msg_final, MAX_LENGTH, 0);  
}

// TODO
// void disconnectClient(int socket) {

// }

void redirection(char* msg, int socket){

  int mp;
  int man;
  regex_t preg;
  const char *mp_regex = "^/mp";
  const char *man_regex = "^/man";
  const char *dc_regex = "^/dc";

  // Commande mp
  mp = regcomp (&preg, mp_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
  if (mp == 0) {
    int match;
    match = regexec (&preg, msg, 0, NULL, 0);
    regfree (&preg);

    if (match == 0) {
      privateMessage(msg, socket);
    }
  }

  // Commande man
  man = regcomp (&preg, man_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
  if (mp == 0) {
    int match;
    match = regexec (&preg, msg, 0, NULL, 0);
    regfree (&preg);

    if (match == 0) {
      commandManual(socket);
    }
  }

  // TODO: Commande dc
  // dc = regcomp (&preg, dc_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
  // if (mp == 0) {
  //   int match;
  //   match = regexec (&preg, msg, 0, NULL, 0);
  //   regfree (&preg);

  //   if (match == 0) {
  //     disconnectClient(socket);
  //   }
  // }
}


void communiquerWithCli(){
  //i++;
  int socket=i;
  printf("Pseudo enregistré : %s , socket : %d\n", pseudos[0], desc[socket]);
  printf("Com avec cli %d\n",socket);
  char msg [MAX_LENGTH] ;
  while(1){
    if (recv(desc[socket], msg, MAX_LENGTH, 0)!=-1) {
      printf("Msg reçu de %s : %s", pseudos[socket], msg);
      if(msg[0]!='/'){
        char* msg_final = (char *) malloc(MAX_LENGTH);
        strcat(msg_final,pseudos[socket]);
        strcat(msg_final, " : ");
        strcat(msg_final, msg);
        
        for(int j=0;j<=i;j++){
          if(desc[socket]!=desc[j]){
            send(desc[j], msg_final, MAX_LENGTH,0);
          }
        }
      }
      else{
        redirection(msg,socket);
      }
    }
  }
  i--;
  shutdown(desc[socket],2); // MAX_CLIENTS
}

void metAjourTableau(int dSC, char* pseudo){
  sem_wait(&semaphore);
  i++;
  // clients[i] = c;
  desc[i] = dSC;
  strncpy(pseudos[i],pseudo,MAX_LENGTH);
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

    /*
    client c;
    c.desc = dSC;
    c.pseudo = pseudo;
    */

    // Ajout du client
    metAjourTableau(dSC, pseudo);
    for(int j=0;j<=i;j++){
    printf("Pseudos %d enregistré : %s \n",desc[j], pseudos[j]);
  } 

    // Communication entre clients
    int t=pthread_create(&thread[0], NULL,(void *)communiquerWithCli,NULL);
    printf("i = %ld \n",i);
  }
  

  printf("Fin du programme \n");
}
