#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <time.h>
#include <unistd.h>

#include "map.h"
#include "voiture.h"
#include "geometrie.h"

#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>

bool running;

#define TPS_ECHANTILLONNAGE 5 //ms

void quitSimulation(int sig){
    printf("\nSIMULATION>>Fin !\n");
    running = false;
}


void jeu(){
    printf("JEU>>Debut de la partie !\n");


    signal(SIGINT, quitSimulation);


    //Recuperation de la memoire partagee
    //Recuperation informations de la carte
    key_t cleBase = ftok("informationCarte", 1);
    int shmid = shmget(cleBase, sizeof(infoBaseMap), 0666);
    infoBaseMap *infoMap = shmat(shmid, NULL, 0666);

    key_t cleBasePoints = ftok("informationCarte", 2);
    int shmidPoint = shmget(cleBasePoints, sizeof(Point) * infoMap->nbPoints, 0666);
    Point* tabPoints = shmat(shmidPoint, NULL, 0666);

    key_t cleBaseLignes = ftok("informationCarte", 3);
    int shmidLigne = shmget(cleBaseLignes, sizeof(Ligne)*infoMap->nbLignes, 0666);
    Ligne* tabLignes = shmat(shmidLigne, NULL, 0666);

    key_t cleBaseChecks = ftok("informationCarte", 4);
    int shmidCheck = shmget(cleBaseChecks, sizeof(Checkpoint)*infoMap->nbCheckpoints, 0666);
    Checkpoint* tabChecks = shmat(shmidCheck, NULL, 0666);

    key_t cleBaseEmpl = ftok("informationCarte", 5);
    int shmidEmpl = shmget(cleBaseEmpl, sizeof(Emplacement)*infoMap->nbEmplacements, 0666);
    Emplacement* tabEmpl = shmat(shmidEmpl, NULL, 0666);

    key_t cleBaseArrivee = ftok("informationCarte", 6);
    int shmidArrivee = shmget(cleBaseArrivee, sizeof(Arrivee), 0666);
    Arrivee* ligneDArrivee = shmat(shmidArrivee, NULL, 0666);

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

    //Demarreur de la course
    key_t cleCourse = ftok("informationCourse", 1);
    int shmidStart = shmget(cleCourse, sizeof(uint8_t), 0666);
    uint8_t *start = shmat(shmidStart, NULL, 0666);

    while (*start != 1)
    {
        usleep(50);
    }
    printf("Simulation>>Debut !\n");
    

    //"Simulation"
    running = true;
    uint64_t delta_t = 0;// en us

    while(running){

        //Recuperation du temps pour la periode d'echantillonage
        struct timeval tempsDeb_t;
        gettimeofday(&tempsDeb_t, NULL);
        
        //Application de la physique sur chaque vehicule
        for(int i = 0; i < *nbJoueur; i++){
            Voiture temp;
            temp = joueur[i];

            //Calcule de la trajectoire
            temp.vitesse = fabs(temp.vitesse + temp.acceleration*TPS_ECHANTILLONNAGE/1000.);
            temp.omega = temp.omega + temp.angleRoue * temp.vitesse * 20*TPS_ECHANTILLONNAGE/1000.;
            temp.x += temp.vitesse * cos(temp.omega);
            temp.y += temp.vitesse * sin(temp.omega);

            //Calcule des colisions
            Rectangle voitureGeo;
            voitureGeo = recupePointVoitureDouble(&temp);
            bool collision = false;
            for(int i = 0; i < infoMap->nbLignes && collision == false; i++){
                if(interRectangleLigne(voitureGeo, tabLignes[i])) collision = true;
            }
            if(collision) joueur[i].vitesse = 0;
            else joueur[i] = temp;

            //Mesure des detecteurs
            mesureDetecteurs(&joueur[i], tabLignes, infoMap->nbLignes);
            
            //Envoi du signal au client
            kill(joueur[i].pid, SIGUSR1);            
        }
        
        //Mise en attente du programme si besoin
        struct timeval tempsFin_t;
        gettimeofday(&tempsFin_t, NULL);
        if(tempsFin_t.tv_sec-tempsDeb_t.tv_sec > 1) delta_t = 2*TPS_ECHANTILLONNAGE*1000;
        else if(tempsFin_t.tv_sec-tempsDeb_t.tv_sec == 1){
            if(tempsFin_t.tv_usec<tempsDeb_t.tv_usec){
                delta_t = tempsDeb_t.tv_usec-tempsFin_t.tv_usec;
                delta_t = 1000000-delta_t;
            }
            else delta_t = 2*TPS_ECHANTILLONNAGE*1000;
        }
        else delta_t = tempsFin_t.tv_usec-tempsDeb_t.tv_usec;
        while (delta_t < TPS_ECHANTILLONNAGE*1000){
            usleep(5);
            gettimeofday(&tempsFin_t, NULL);
            if(tempsFin_t.tv_sec-tempsDeb_t.tv_sec > 1) delta_t = 2*TPS_ECHANTILLONNAGE*1000;
            else if(tempsFin_t.tv_sec-tempsDeb_t.tv_sec == 1){
                if(tempsFin_t.tv_usec<tempsDeb_t.tv_usec){
                    delta_t = tempsDeb_t.tv_usec-tempsFin_t.tv_usec;
                    delta_t = 1000000-delta_t;
                }
                else delta_t = 2*TPS_ECHANTILLONNAGE*1000;
            }
            else delta_t = tempsFin_t.tv_usec-tempsDeb_t.tv_usec;
        }
    }

    //Liberation des segments de memoire partagee
    shmdt(tabPoints);
    shmdt(tabLignes);
    shmdt(tabChecks);
    shmdt(tabEmpl);
    shmdt(ligneDArrivee);
    shmdt(infoMap);

    shmdt(joueur);
    shmdt(nbJoueurMax);
    shmdt(nbJoueur);
    shmctl(shmidPoint, IPC_RMID, NULL);
    shmctl(shmidLigne, IPC_RMID, NULL);
    shmctl(shmidCheck, IPC_RMID, NULL);
    shmctl(shmidEmpl, IPC_RMID, NULL);
    shmctl(shmidArrivee, IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);


    return;
}