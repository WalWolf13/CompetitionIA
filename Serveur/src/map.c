#include "map.h"

#include "erreur.h"
#include "texte.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/shm.h>

char *concatene(char *a, char*b){
    int lenA, lenB;
    lenA = strlen(a);
    lenB = strlen(b);
    int lenR = lenA+lenB;
    char *resultat = malloc(sizeof(char)*(lenR+1));
    int i = 0;
    for(;i < lenA; i++) resultat[i] = a[i];
    for(int j = 0; j < lenB; j++) resultat[i+j] = b[j];
    resultat[lenR] = '\0';
    return resultat;
}

int chargeFichierMap(char *fileName){
    char *path = concatene(PATH_FOLDER, fileName);
    FILE *fichier = fopen(path, "r");
    if(fichier == NULL){
        AFFICHE_ERREUR(" Impossible d'ouvrir le fichier ");
        printf("\"%s\"\n", path);
        return -1;
    }
    free(path);
    chaineLexical maChaine = lexer(fichier);
    fclose(fichier);
    resultatGrammaire r = analyseGrammatical(maChaine);

    //Ajout des informations recuperer par l'annalyseur grammatical en memoire partage
    key_t cleBase = ftok("informationCarte", 1);
    int shmidInfoMap = shmget(cleBase, sizeof(infoBaseMap), IPC_EXCL | IPC_CREAT | 0666);
    infoBaseMap *inf = shmat(shmidInfoMap, NULL, 0666);
    inf->nbCheckpoints = r.nbCheckpoints;
    inf->nbEmplacements = r.nbEmplacements;;
    inf->nbLignes = r.nbLignes;
    inf->nbPoints = r.nbPoints;

    key_t cleBasePoints = ftok("informationCarte", 2);
    int shmidPoint = shmget(cleBasePoints, sizeof(Point)*inf->nbPoints, IPC_EXCL | IPC_CREAT | 0666);
    Point* tabPoints = shmat(shmidPoint, NULL, 0666);

    key_t cleBaseLignes = ftok("informationCarte", 3);
    int shmidLigne = shmget(cleBaseLignes, sizeof(Ligne)*inf->nbLignes, IPC_EXCL | IPC_CREAT | 0666);
    Ligne* tabLignes = shmat(shmidLigne, NULL, 0666);

    key_t cleBaseChecks = ftok("informationCarte", 4);
    int shmidCheck = shmget(cleBaseChecks, sizeof(Checkpoint)*inf->nbCheckpoints, IPC_EXCL | IPC_CREAT | 0666);
    Checkpoint* tabChecks = shmat(shmidCheck, NULL, 0666);

    key_t cleBaseEmpl = ftok("informationCarte", 5);
    int shmidEmpl = shmget(cleBaseEmpl, sizeof(Emplacement)*inf->nbEmplacements, IPC_EXCL | IPC_CREAT | 0666);
    Emplacement* tabEmpl = shmat(shmidEmpl, NULL, 0666);

    key_t cleBaseArrivee = ftok("informationCarte", 6);
    int shmidArrivee = shmget(cleBaseArrivee, sizeof(Arrivee), IPC_EXCL | IPC_CREAT | 0666);
    Arrivee* ligneDArrivee = shmat(shmidArrivee, NULL, 0666);

    extractionInformationMap(maChaine, r, tabPoints, tabLignes, tabChecks, tabEmpl, ligneDArrivee);

    shmdt(tabPoints);
    shmdt(tabLignes);
    shmdt(tabChecks);
    shmdt(tabEmpl);
    shmdt(ligneDArrivee);
    shmdt(inf);
    freeChaineLexical(maChaine);

    return 0;
}