//
// Created by Romain Mezghenna on 19/05/2022.
//

int trouverPseudo(char* pseudo);

void privateMessage(char* msg, int socket);

void commandManual(int socket);

void disconnectClient(int socket);

void envoyerListClients(int socket);

void envoyerListCanaux(int socket);

void redirection(char* msg, int socket);
