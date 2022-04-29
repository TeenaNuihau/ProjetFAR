#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void main (int argc, char* argv[]) {

    char* pseudoR = "mat\n";

    char pseudo[strlen(pseudoR)-1];
    strncpy(pseudo,&pseudoR[0],strlen(pseudoR)-1);
    printf("Pseudo reçu : %s \n", pseudo);
}
