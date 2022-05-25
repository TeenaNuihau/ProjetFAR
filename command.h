//
// Created by Romain Mezghenna on 19/05/2022.
//

int trouverPseudo(char* pseudo);

void privateMessage(char* msg, int dSC_from, Liste *liste);

void commandManual(int socket);

void disconnectClient(int socket, Liste *liste);

void envoyerListClients(int socket, Liste *liste);

void envoyerListCanaux(int socket);

void redirection(char* msg, int socket, Liste *liste);
