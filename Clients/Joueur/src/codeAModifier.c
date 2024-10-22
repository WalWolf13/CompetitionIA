#include "competitionIA.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <math.h>

detect initialisation(){
    detect detecteur;
    detecteur.nbDetecteur = 4; //Choisir le nombre de detecteur voulu entre 1 et 10 inclu
    //Configuration de chaque detecteur :
    detecteur.detecteur[0] = -180;
    detecteur.detecteur[1] = 45.;
    detecteur.detecteur[2] = -45.;
    detecteur.detecteur[3] = 0.;

    return detecteur;
}


//Contrôle le comportement de la voiture selon les valeurs reçues
void controle(entreeControleur *entree, sortieControleur *sortie){
    
    sortie->sens = MARCHE_AVANT;
    double vitesseConsigne = 100;// m/s
    double vitesse = entree->vitesse;

    double accelF = (vitesseConsigne - abs(vitesse))*750;


    double esp = entree->detecteur[2]-entree->detecteur[1];
    esp = -(3./2.)*esp;

    sortie->angleRoues = esp;
    sortie->acceleration = accelF;

}