#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "afficheur.h"
#include "joueur.h"


void reception(int connect){
    char c = 0;
    recv(connect, &c, 1, 0);
    //Rajouter une condition qui dit bloque a 1 afficheur !
    if(c == 'V') afficheur_communication(connect);
    else if(c == 'J') nouveau_joueur(connect);
    close(connect);
    exit(0);
}

void accueil(){
    bool continu = true;

    int nbConnection = 0;

    int nbClient = 5;
    printf("ACCUEIL>>Accueil des clients !\n");
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in socketAddr;
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(25566);
    socketAddr.sin_addr.s_addr = INADDR_ANY;
    int tailleSockAddr = sizeof(socketAddr);
    printf("ACCUEIL>>Mise sur ecoute\n");
    bind(sock, (struct sockaddr *) &socketAddr, tailleSockAddr);
    if(socketAddr.sin_port != htons(25566)) printf("Erreur de port !\n");
    listen(sock, nbClient);
    while(continu){
        printf("ACCUEIL>>Ecoute...\t\t(n°%d)\n", nbConnection);
        nbConnection++;
        int connect = accept(sock, (struct sockaddr *) &socketAddr, (socklen_t *) &tailleSockAddr);
        switch (fork())
        {
        case 0:
            reception(connect);
            break;
        
        default:
            break;
        }

    }
    close(sock);
    exit(0);
}