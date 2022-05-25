//
// Created by Romain Mezghenna on 19/05/2022.
//

#include <string.h>
#include <regex.h>
#include "command.h"
#include "list.h"

// send private message
void privateMessage(char* msg, int socket) {
    char delim[] = " ";
    char *p = strtok(msg, delim);
    p = strtok(NULL, delim);
    char* p_desti = p;
    int dSC_desti = rechercherElementPseudo(listeClient, p)->dSC;

    char* msg_final = (char *) malloc(MAX_LENGTH);
    strcat(msg_final, "(privé) ");
    strcat(msg_final, p_desti);
    strcat(msg_final, " : ");
    p = strtok(NULL, delim);

    while ( p != NULL ) {
        strcat(msg_final, " ");
        strcat(msg_final, p);
        p = strtok(NULL, delim);
    }

    send(dSC_desti,msg_final,MAX_LENGTH,0);
    free(msg_final);
}

// display the commands list
void commandManual(int socket) {
    char* msg_final = "/man : display the commands list \n/mp username msg : send private message \n/list : display the list of connected client(s) \n/dc : disconnect \n";
    send(socket, msg_final, MAX_LENGTH, 0);
}

void disconnectClient(int socket, Liste *liste) {
    supprimerElement(liste, socket);
    shutdown(socket, 2);
}

void envoyerListClients(int socket, Liste *liste) {
    char* msg_final = (char *) malloc(MAX_LENGTH*MAX_CLIENTS+200);
    strcat(msg_final,"Liste des clients : \n");

    element* tmp = liste->head;
    while (tmp!=NULL) {
        strcat(msg_final,tmp->pseudo);
        strcat(msg_final," \n");
    }

    send(desc[socket],msg_final,MAX_LENGTH,0);
    free(msg_final);
}

void redirection(char* msg, int socket) {
    int mp;
    int man;
    int list;
    int dc;
    regex_t preg;
    const char *mp_regex = "^/mp";
    const char *man_regex = "^/man";
    const char *dc_regex = "^/dc";
    const char *list_regex = "^/list";

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
    if (man == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            commandManual(socket);
        }
    }

    // Commande dc
    dc = regcomp (&preg, dc_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if (dc == 0) {
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if (match == 0) {
            disconnectClient(socket);
        }
    }

    // Commande list
    list = regcomp (&preg, list_regex, REG_NOSUB | REG_EXTENDED | REG_ICASE);
    if(list == 0){
        int match;
        match = regexec (&preg, msg, 0, NULL, 0);
        regfree (&preg);

        if(match == 0){
            envoyerListClients(socket);
        }
    }
}


/*void envoyerListCanaux(int socket) {

}
*/