#include "color.h"
#include <stdio.h>
void printrouge(char * msg){
    printf("\033[31m" );
    printf("%s",msg);
    printf("\033[37m" );
}

void printbleu(char * msg){
    printf("\033[34m" );
    printf("%s",msg);
    printf("\033[37m" );
}

void printvert(char * msg){
    printf("\033[32m" );
    printf("%s",msg);
    printf("\033[37m" );
}

void printorange(char * msg){
    printf("\033[33m" );
    printf("%s",msg);
    printf("\033[37m" );
}
void printcyanfile(char* msg, int num) {
    printf("\033[36m" );
    printf("%d : %s\n",num,msg);
    printf("\033[37m" );
}

void printcyan(char* msg) {
    printf("\033[36m" );
    printf("%s",msg);
    printf("\033[37m" );
}

void printcyanint(int n){
    printf("\033[36m" );
    printf("%d",n);
    printf("\033[37m" );
}