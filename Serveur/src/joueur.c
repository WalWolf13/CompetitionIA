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
#include <sys/sem.h>
#include <signal.h>

#include <time.h>

#include "voiture.h"

int socket_client;
Voiture *voiture;

void envoiInformation(int signal){
    for(int i = 0; i < voiture->nbDetecteur; i++){
        uint16_t msg = ((double) voiture->distDetecteur[i])*65535/DISTANCE_MAX_DETECTEUR_VOITURE;
        //if(voiture->distDetecteur[i] >= DISTANCE_MAX_DETECTEUR_VOITURE) msg = 65535;
        send(socket_client, &msg, 2, 0);
    }
    uint8_t marche = voiture->marche_av_ar;
    uint16_t vitesse = voiture->vitesse*100;
    uint16_t angleRoue = (voiture->angleRoue*4*32768/pi)+32768;
    uint8_t msg = 0;
    send(socket_client, &marche, 1, 0);
    send(socket_client, &vitesse, 2, 0);
    send(socket_client, &angleRoue, 2, 0);
    send(socket_client, &msg, 1, 0);
}

void nouveau_joueur(int connect){
    socket_client = connect;
    //Verification que la connection est bien pour un joueur
    uint8_t c = 0;
    recv(connect, &c, 1, 0);
    if(c != 'O') return;
    recv(connect, &c, 1, 0);
    if(c != 'U') return;

    uint8_t msg[3] = {'J', 'O', 'K'};
    send(connect, msg, 3, 0);
    
    int id = 0;
    //Recupération de la memoire partagée
        //Recuperation informations joueurs
    key_t cleNbJoueurMax = ftok("informationJoueur", 1);
    key_t cleNbJoueur = ftok("informationJoueur", 2);
    key_t cleJoueur = ftok("informationJoueur", 3);
    int shmidNbJoueurMax = shmget(cleNbJoueurMax, sizeof(int), 0666);
    int shmidNbJoueur = shmget(cleNbJoueur, sizeof(int), 0666);

    int *nbJoueurMax = shmat(shmidNbJoueurMax, NULL, 0666);
    int *nbJoueur = shmat(shmidNbJoueur, NULL, 0666);

    int shmidJoueur = shmget(cleJoueur, sizeof(Voiture)*(*nbJoueurMax), 0666);
    Voiture *joueur = shmat(shmidJoueur, NULL, 0666);
    int semidJoueur = semget(cleJoueur, 1, 0666);

    //Demarreur de la course
    key_t cleCourse = ftok("informationCourse", 1);
    int shmidStart = shmget(cleCourse, sizeof(uint8_t), 0666);
    uint8_t *start = shmat(shmidStart, NULL, 0666);

    struct sembuf sem_oper;
    sem_oper.sem_num = 0;
    sem_oper.sem_flg = 0;
    sem_oper.sem_op = 1;
    
    
    
    uint8_t nbCaractereNomDuJoueur = 0;
    recv(connect, &nbCaractereNomDuJoueur, 1, 0);

    semop(semidJoueur, &sem_oper, 1);
        if(*nbJoueur >= *nbJoueurMax){
            uint8_t msg = 255;
            send(connect, &msg, 1, 0);
            close(connect);
            exit(-1);
        }
        id = *nbJoueur;
        *nbJoueur += 1;

        sem_oper.sem_op = -1;
    semop(semidJoueur, &sem_oper, 1);

    joueur[id].id = id;
    joueur[id].pid = getpid();
    recv(connect, &joueur[id].nom, nbCaractereNomDuJoueur, 0);
    joueur[id].nom[nbCaractereNomDuJoueur] = '\0';
    recv(connect, &joueur[id].nbDetecteur, 1, 0);
    if(joueur[id].nbDetecteur < 0 || joueur[id].nbDetecteur > 10){
        uint8_t msg = 255;
        send(connect, &msg, 1, 0);
        close(connect);
        exit(-1);
    }
    for(int i = 0; i < joueur[id].nbDetecteur; i++){
        uint16_t msg;
        recv(connect, &msg, 2, 0);

        joueur[id].detecteur[i] = ((float) msg - 32768.) * pi / 32768.;
    }

    {
        uint8_t msg = 0;
        send(connect, &msg, 1, 0);
    }


    //teste affichage des valeurs :
    printf("nouveau Joueur :\n");
    printf("\tNumero : %d\n", id);
    printf("\tNom : %s\n", joueur[id].nom);
    printf("\tNombre de detecteur : %d\n", joueur[id].nbDetecteur);
    for(int i = 0; i < joueur[id].nbDetecteur; i++){
        printf("\tDetecteur %d : %f\n", i, joueur[id].detecteur[i]);
    }

    voiture = &joueur[id];
    signal(SIGUSR1, envoiInformation);

    while(*start != 1){
        usleep(50);
    }
    printf("[%d]>>Debut de course !\n", id);
    
    {
        uint8_t msg = 1;
        send(connect, &msg, 1, 0);
    }
    while(*start == 1){
        uint8_t msg0 = 0;
        uint16_t msg1 = 0;
        int8_t msg2 = 0;
        recv(connect, &msg0, 1, 0);
        voiture->marche_av_ar = msg0;
        recv(connect, &msg2, 1, 0);
        voiture->acceleration = msg2/10.;
        recv(connect, &msg1, 2, 0);
        voiture->angleRoue = (pi)*((double)(msg1-32768))/32768;
    }
    
}