#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <wait.h>

#include "accueil.h"
#include "afficheur.h"
#include "joueur.h"

int sock;
int nbConnexion;
int nbDeconnexion;

/*
    La reception dirige la connection arrivante vers le bon service (connection d'un nouveau joueur ou de l'afficheur).
*/
void reception(int connect){

    signal(SIGINT, SIG_IGN);

    char c = 0;
    recv(connect, &c, 1, 0);
    //Rajouter une condition qui dit bloque a 1 afficheur !
    if(c == 'V') afficheur_communication(connect);
    else if(c == 'J') nouveau_joueur(connect);
    close(connect);
    exit(0);
}


/*
    Reception des deconnexions
*/
void fils_accueil_mort(int sig){
    nbDeconnexion++;
}

/*
    Fermeture de l'accueil
*/
void close_accueil(int sig){
    close(sock);
    while(nbConnexion != nbDeconnexion){
        wait(NULL);
    }
    exit(0);
}

/*
    La fonction accueil permet d'accueillir les connections TCP IPv4 du serveur afin de les dirigees vers la receptions.
*/
void accueil(){

    signal(SIGINT, close_accueil);
    signal(SIGCHLD, fils_accueil_mort);

    bool continu = true;

    nbConnexion = 0;
    nbDeconnexion = 0;

    int nbClient = 5;
    printf("ACCUEIL>>Accueil des clients !\n");
    sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in socketAddr;
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(PORT_SERVEUR);
    socketAddr.sin_addr.s_addr = INADDR_ANY;
    int tailleSockAddr = sizeof(socketAddr);
    printf("ACCUEIL>>Mise sur ecoute\n");
    bind(sock, (struct sockaddr *) &socketAddr, tailleSockAddr);
    if(socketAddr.sin_port != htons(PORT_SERVEUR)) printf("Erreur de port !\n");
    listen(sock, nbClient);
    while(continu){
        printf("ACCUEIL>>Ecoute...\t\t(n°%d)\n", nbConnexion);
        int connect = accept(sock, (struct sockaddr *) &socketAddr, (socklen_t *) &tailleSockAddr);
        nbConnexion++;
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