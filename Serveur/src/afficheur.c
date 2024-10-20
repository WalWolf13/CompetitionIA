#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "erreur.h"

#include "map.h"
#include "voiture.h"

void afficheur_communication(int connect){
    // Verification que la connection est bien pour l'afficheur
    uint8_t c = 0;
    recv(connect, &c, 1, 0);
    if (c != 'I')
        return;
    recv(connect, &c, 1, 0);
    if (c != 'S')
        return;
    recv(connect, &c, 1, 0);
    if (c != 'I')
        return;
    recv(connect, &c, 1, 0);
    if (c != 'O')
        return;

    // Reponse du serveur
    uint8_t msg[3] = {'V', 'O', 'K'};
    send(connect, msg, 3, 0);
    printf("AFFICHEUR>>Connecter !\n");


    // Recuperation de la memoire partagee
    
    //Information de la map
    key_t cleBase = ftok("informationCarte", 1);
    int shmid = shmget(cleBase, sizeof(infoBaseMap), 0666);
    infoBaseMap *infoMap = shmat(shmid, NULL, 0666);

    key_t cleBaseLignes = ftok("informationCarte", 3);
    int shmidLigne = shmget(cleBaseLignes, sizeof(Ligne) * infoMap->nbLignes, 0666);
    Ligne *tabLignes = shmat(shmidLigne, NULL, 0666);

    //Information sur les joueurs
    key_t cleNbJoueurMax = ftok("informationJoueur", 1);
    key_t cleNbJoueur = ftok("informationJoueur", 2);
    key_t cleJoueur = ftok("informationJoueur", 3);
    int shmidNbJoueurMax = shmget(cleNbJoueurMax, sizeof(int), 0666);
    int shmidNbJoueur = shmget(cleNbJoueur, sizeof(int), 0666);

    int *nbJoueurMax = shmat(shmidNbJoueurMax, NULL, 0666);
    int *nbJoueur = shmat(shmidNbJoueur, NULL, 0666);

    int shmidJoueur = shmget(cleJoueur, sizeof(Voiture)*(*nbJoueurMax), 0666);
    Voiture *joueur = shmat(shmidJoueur, NULL, 0666);

    //Demarreur de la course
    key_t cleCourse = ftok("informationCourse", 1);
    int shmidStart = shmget(cleCourse, sizeof(uint8_t), 0666);
    uint8_t *start = shmat(shmidStart, NULL, 0666);
    uint8_t dejaDemarrer = 0;

    while (c != 255){
        if (recv(connect, &c, 1, 0) == -1)
            c = 255;
        if(dejaDemarrer == 1 && *start != 1) c = 255;
        switch (c){
        case 1: {// Renvoi sur 2 octets du nombre de lignes
            uint16_t mem = (uint16_t)infoMap->nbLignes;
            send(connect, &mem, 2, 0);
        }
        break;

        case 2:{ // Envoi de tous les segments de mur
            uint32_t mem = 0;
            for (int i = 0; i < infoMap->nbLignes; i++){
                mem = (uint32_t)(tabLignes[i].a.x * 1000.);
                send(connect, &mem, 4, 0);
                mem = (uint32_t)(tabLignes[i].a.y * 1000.);
                send(connect, &mem, 4, 0);
                mem = (uint32_t)(tabLignes[i].b.x * 1000.);
                send(connect, &mem, 4, 0);
                mem = (uint32_t)(tabLignes[i].b.y * 1000.);
                send(connect, &mem, 4, 0);
            }
        }
        break;

        case 3:{ // Renvoi sur 1 octet le nombre de joueurs
            uint8_t mem = *nbJoueur;
            send(connect, &mem, 1, 0);
        }
        break;

        case 4:{// Envoi la position de chaque joueur
            uint32_t mem = 0;
            uint16_t mem1 = 0;
            for(int i = 0; i < *nbJoueur; i++){
                mem = (uint32_t)(joueur[i].x*1000.);
                send(connect, &mem, 4, 0);
                mem = (uint32_t)(joueur[i].y*1000.);
                send(connect, &mem, 4, 0);
                mem1 = (uint32_t) (10430.*joueur[i].omega+32768);
                send(connect, &mem1, 2, 0);
            }
        
        }
        break;

        case 5: // Demarre la course
            if(dejaDemarrer == 0){
                *start = 1;
                dejaDemarrer = 1;
            }
            break;

        case 255:
            c = 255;
            send(connect, &c, 1, 0);
            break;
        }
    }

    shmdt(start);
    shmdt(tabLignes);
    shmdt(infoMap);

    return;
}