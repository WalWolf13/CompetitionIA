#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <time.h>
#include <unistd.h>

#include "map.h"
#include "voiture.h"
#include "geometrie.h"
#include "physique.h"

#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>

bool running;

uint64_t calcule_difference_temps_us(struct timeval tempsDeb_t, struct timeval tempsFin_t){
    uint64_t delta_t;
    if(tempsFin_t.tv_sec-tempsDeb_t.tv_sec > 1) delta_t = 2*TPS_ECHANTILLONNAGE*1000;
        else if(tempsFin_t.tv_sec-tempsDeb_t.tv_sec == 1){
            if(tempsFin_t.tv_usec<tempsDeb_t.tv_usec){
                delta_t = tempsDeb_t.tv_usec-tempsFin_t.tv_usec;
                delta_t = 1000000-delta_t;
            }
            else delta_t = 2*TPS_ECHANTILLONNAGE*1000;
        }
    else delta_t = tempsFin_t.tv_usec-tempsDeb_t.tv_usec;
    return delta_t;
}

void quitter_simulation(int sig){
    printf("\nSIMULATION>>Fin !\n");
    running = false;
}


bool verification_checkpoint_voiture(Voiture *v, Checkpoint *listeCheckpoint, uint16_t nbCheckpoint, Arrivee* ligneDArrivee){
    Rectangle rect_voiture = recupere_point_Voiture_double(v);
    if(v->numeroDeCheckpoint >= nbCheckpoint){
        Ligne l;
        l.a.x = ligneDArrivee->a.x;
        l.a.y = ligneDArrivee->a.y;
        l.b.x = ligneDArrivee->b.x;
        l.b.y = ligneDArrivee->b.y;
        if(intersection_rectangle_ligne(rect_voiture, l)){
            //printf("Voiture[%d]>> J'ai fini !\n", v->pid);
            return true;
        }
    }
    else{
        Ligne l;
        l.a.x = listeCheckpoint[v->numeroDeCheckpoint].a.x;
        l.a.y = listeCheckpoint[v->numeroDeCheckpoint].a.y;
        l.b.x = listeCheckpoint[v->numeroDeCheckpoint].b.x;
        l.b.y = listeCheckpoint[v->numeroDeCheckpoint].b.y;
        if(intersection_rectangle_ligne(rect_voiture, l)){
            //printf("Voiture[%d]>> Je vient de passer le checkpoint %d\n", v->pid, v->numeroDeCheckpoint);
            v->numeroDeCheckpoint += 1;
        }
    }
    return false;
}

void verification_checkpoint(Voiture *v, int nbVoiture, Checkpoint *listeCheckpoint, uint16_t nbCheckpoint, Arrivee* ligneDArrivee, uint64_t nbIteration){
    for(int i = 0; i < nbVoiture; i++){
            if(verification_checkpoint_voiture(&v[i], listeCheckpoint, nbCheckpoint, ligneDArrivee)){
                v[i].fini = true;
                v[i].x = -1;
                v[i].y = -1;
                v[i].tempDeFin = nbIteration;
                printf("Voiture[%d]>> J'ai fini en %f s\n", v[i].pid, ((double)nbIteration)*0.005 );
            }
    }
}

void jeu(){
    printf("JEU>>Debut de la partie !\n");

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
    
    signal(SIGINT, quitter_simulation);

    //"Simulation"
    running = true;
    uint64_t delta_t = 0;// en us
    uint64_t nbIteration = 0;


    uint64_t delta_t_max = 0;

    while(running){

        //Recuperation du temps pour la periode d'echantillonage
        struct timeval tempsDeb_t;
        gettimeofday(&tempsDeb_t, NULL);
        
        //Application de la physique sur chaque vehicule
        avancer_monde(joueur, *nbJoueur, infoMap, tabLignes);

        //Verification des passages aux checkpoints
        verification_checkpoint(joueur, *nbJoueur, tabChecks, infoMap->nbCheckpoints, ligneDArrivee, nbIteration);

        //Mise en attente du programme si besoin
        struct timeval tempsFin_t;
        gettimeofday(&tempsFin_t, NULL);
        delta_t = calcule_difference_temps_us(tempsDeb_t, tempsFin_t);
        printf("Deltat_t max : %ld µs\n", delta_t_max);
        if(delta_t > delta_t_max) delta_t_max = delta_t;
        while (delta_t < TPS_ECHANTILLONNAGE*1000){
            usleep(TPS_ECHANTILLONNAGE);
            gettimeofday(&tempsFin_t, NULL);
            delta_t = calcule_difference_temps_us(tempsDeb_t, tempsFin_t);
        }
        nbIteration++;
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
    
    return;
}