#include <stdio.h>
#include <stdlib.h>
#include "voiture.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>



#include <time.h>

#include "erreur.h"
#include "accueil.h"
#include "map.h"
#include "jeu.h"


void fin_de_programme(int sig){

    printf("Fermeture du serveur...\n");

    key_t cleBase = ftok("informationCarte", 1);
    int shmidMap = shmget(cleBase, sizeof(infoBaseMap), 0666);
    infoBaseMap *infoMap = shmat(shmidMap, NULL, 0666);

    key_t cleBasePoints = ftok("informationCarte", 2);
    int shmidPoint = shmget(cleBasePoints, sizeof(Point) * infoMap->nbPoints, 0666);
    
    key_t cleBaseLignes = ftok("informationCarte", 3);
    int shmidLigne = shmget(cleBaseLignes, sizeof(Ligne)*infoMap->nbLignes, 0666);
    
    key_t cleBaseChecks = ftok("informationCarte", 4);
    int shmidCheck = shmget(cleBaseChecks, sizeof(Checkpoint)*infoMap->nbCheckpoints, 0666);

    key_t cleBaseEmpl = ftok("informationCarte", 5);
    int shmidEmpl = shmget(cleBaseEmpl, sizeof(Emplacement)*infoMap->nbEmplacements, 0666);

    key_t cleBaseArrivee = ftok("informationCarte", 6);
    int shmidArrivee = shmget(cleBaseArrivee, sizeof(Arrivee), 0666);

    key_t cleNbJoueurMax = ftok("informationJoueur", 1);
    key_t cleNbJoueur = ftok("informationJoueur", 2);
    key_t cleJoueur = ftok("informationJoueur", 3);
    int shmidNbJoueurMax = shmget(cleNbJoueurMax, sizeof(int), 0666);
    int shmidNbJoueur = shmget(cleNbJoueur, sizeof(int), 0666);
    int *nbJoueurMax = shmat(shmidNbJoueurMax, NULL, 0666);
    int shmidJoueur = shmget(cleJoueur, sizeof(Voiture)*(*nbJoueurMax), 0666);

    key_t cleCourse = ftok("informationCourse", 1);
    int shmidStart = shmget(cleCourse, sizeof(uint8_t), IPC_CREAT | 0666);

    int semidJoueur = semget(cleJoueur, 1, IPC_CREAT | 0666);
    semctl(semidJoueur, 0, SETVAL, 1);

    shmdt(nbJoueurMax);
    shmdt(infoMap);

    shmctl(shmidPoint, IPC_RMID, NULL);
    shmctl(shmidLigne, IPC_RMID, NULL);
    shmctl(shmidCheck, IPC_RMID, NULL);
    shmctl(shmidEmpl, IPC_RMID, NULL);
    shmctl(shmidArrivee, IPC_RMID, NULL);
    shmctl(shmidMap, IPC_RMID, NULL);

    shmctl(shmidNbJoueur, IPC_RMID, NULL);
    shmctl(shmidNbJoueurMax, IPC_RMID, NULL);
    shmctl(shmidStart, IPC_RMID, NULL);
    shmctl(shmidJoueur, IPC_RMID, NULL);

    semctl(semidJoueur, 0, IPC_RMID);

    printf("Serveur ferme !\n");

    exit(0);
}

int main(int argc, char *argv[]){


    //Gestion des erreurs d'entree

    //Nombre d'argument
    if(argc != 2){
        AFFICHE_ERREUR(" l'execution du serveur necessite un argument qui est le nombre de joueur\n");
        exit(-1);
    }

    //L'argurment doit etre un nombre
    int nombreDeParticipant = 0;
    char* c = argv[1];
    while(*c != '\0'){
        if(*c > '9' || *c < '0'){
            AFFICHE_ERREUR(" l'execution du serveur necessite un argument qui est un ");
            printf("\033[38;2;255;0;0m");//Ecriture en rouge
            printf("NOMBRE");
            printf("\033[m");//Ecriture de couleur normale
            printf(" de joueur\n");
            exit(-2);
        }
        nombreDeParticipant *= 10;
        nombreDeParticipant += *c - '0';
        c++;
    }
    printf("Serveur>> Nombre de joueur : %d\n", nombreDeParticipant);

    //Lecture de la MAP
    charger_fichier_map("carte0.m");

    key_t cleBase = ftok("informationCarte", 1);
    int shmidInfoMap = shmget(cleBase, sizeof(infoBaseMap), 0666);
    infoBaseMap *inf = shmat(shmidInfoMap, NULL, 0666);

    key_t cleBaseEmpl = ftok("informationCarte", 5);
    int shmidEmpl = shmget(cleBaseEmpl, sizeof(Emplacement)*inf->nbEmplacements, 0666);
    Emplacement* tabEmpl = shmat(shmidEmpl, NULL, 0666);


    //Initialisation des voitures dans la memoire partagee des processus
    key_t cleNbJoueurMax = ftok("informationJoueur", 1);
    int shmidNbJoueurMax = shmget(cleNbJoueurMax, sizeof(int), IPC_EXCL | IPC_CREAT | 0666);
    int *nbJoueurMax = shmat(shmidNbJoueurMax, NULL, 0666);
    *nbJoueurMax = nombreDeParticipant;
    shmdt(nbJoueurMax);

    key_t cleNbJoueur = ftok("informationJoueur", 2);
    int shmidNbJoueur = shmget(cleNbJoueur, sizeof(int), IPC_EXCL | IPC_CREAT | 0666);
    int *nbJoueur = shmat(shmidNbJoueur, NULL, 0666);
    *nbJoueur = 0;
    shmdt(nbJoueur);

    key_t cleJoueur = ftok("informationJoueur", 3);
    int shmid = shmget(cleJoueur, sizeof(Voiture)*nombreDeParticipant, IPC_EXCL | IPC_CREAT | 0666);
    Voiture *joueur = shmat(shmid, NULL, 0666);
    for(int i = 0; i < nombreDeParticipant; i++){
        sprintf(joueur[i].nom, "Joueur %d", i);
        joueur[i].angleRoue = 0;
        joueur[i].largeur = LARGEUR_VOITURE;
        joueur[i].longueur = LONGUEUR_VOITURE;
        joueur[i].masse = MASSE_VOITURE;
        joueur[i].omega = tabEmpl[i].angle;
        joueur[i].vitesse = 0;
        joueur[i].x = tabEmpl[i].a.x;
        joueur[i].y = tabEmpl[i].a.y;
    }
    shmdt(joueur);

    shmdt(tabEmpl);
    shmdt(inf);


    //Initialisation du demarreur de la course
    key_t cleCourse = ftok("informationCourse", 1);
    int shmidStart = shmget(cleCourse, sizeof(uint8_t), IPC_EXCL | IPC_CREAT | 0666);
    uint8_t *start = shmat(shmidStart, NULL, 0666);
    *start = 0;
    shmdt(start);

    //semaphore
    int semidJoueur = semget(cleJoueur, 1, IPC_EXCL | IPC_CREAT | 0666);
    semctl(semidJoueur, 0, SETVAL, 1);


    switch (fork()){
    case 0:
        accueil();

        break;
    case -1:
        AFFICHE_ERREUR(" Creation du processus d'accueil du serveur a echoue\n");
        exit(-3);
    default:
        signal(SIGINT, fin_de_programme);
        jeu();
        break;
    }

    /*semctl(semidJoueur, 0, IPC_RMID);
    shmctl(shmidNbJoueur, IPC_RMID, NULL);
    shmctl(shmidNbJoueurMax, IPC_RMID, NULL);
    shmctl(shmidStart, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);*/

    fin_de_programme(0);
}