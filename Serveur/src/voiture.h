#ifndef __VOITURE_H__
#define __VOITURE_H__

#include <stdint.h>
#include "geometrie.h"
#include <sys/types.h>


#define LONGUEUR_VOITURE (double) 5.5 //m
#define LARGEUR_VOITURE (double) 1.8 //m
#define MASSE_VOITURE (double) 750. //kg
#define ANGLE_AVE (double) atan(LARGEUR_VOITURE/LONGUEUR_VOITURE) //randian
#define ANGLE_BVE (double) pi-ANGLE_BVE //randian
#define DISTANCE_MAX_DETECTEUR_VOITURE (double) 20.0 //m

    typedef enum Voiture_Marche{
        VOITURE_M_AVANT, VOITURE_M_ARRIERE
    } Voiture_Marche;

    typedef struct Voiture{
        //Caracteristiques :
        char nom[256];
        double longueur, largeur;//En metre
        double omega;//Angle de la voiture en radians
        double x, y;//Coordonnees du centre de la voiture en m
        double masse;
        pid_t pid;
        int id;

        //Dynamique :
        Voiture_Marche marche_av_ar;
        double acceleration;
        double vitesse;
        double angleRoue;

        //Detecteur
        int nbDetecteur;
        double detecteur[10];
        double distDetecteur[10];

        
    } Voiture;

    typedef struct PointsVoiture{
        uint32_t xa, ya;
        uint32_t xb, yb;
        uint32_t xc, yc;
        uint32_t xd, yd;
    } PointsVoiture;

Voiture* initVoiture(double longueur, double largeur, double omega, double x, double y);
PointsVoiture *recupePointVoiture(Voiture *v);
Rectangle recupePointVoitureDouble(Voiture *v);
void  mesureDetecteurs(Voiture *v, Ligne *mur, int nbMur);
void freeVoiture(Voiture *v);

#endif