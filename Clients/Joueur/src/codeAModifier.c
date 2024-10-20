#include "competitionIA.h"

#include <stdio.h>
#include <stdlib.h>

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

    static int arrive = 0;
    static int marcheArriere = 0;
    static int v_zero = 0;
    sortie->sens = MARCHE_AVANT;
    double vitesseConsigne = 1000;// m/s
    double vitesse = entree->vitesse;
    
    if((vitesse < 0.1) && marcheArriere == 0){
        v_zero++;
    }
    if(v_zero == 10 && marcheArriere == 0){
        v_zero = 10;
        marcheArriere = 1;
    }
    
    if(vitesse > 85) arrive = 1;
    if(arrive) vitesseConsigne = 27;
    double accelF = (vitesseConsigne - abs(vitesse))*750;


    double esp = entree->detecteur[2]-entree->detecteur[1];
    esp = -(3./2.)*esp;
    if(marcheArriere){
        sortie->sens = MARCHE_ARRIERE;
        v_zero--;
        esp =  -100*esp;
        if(v_zero <= 0){
            v_zero = 0;
            marcheArriere = 0;
        }
    }


    sortie->angleRoues = esp;
    sortie->acceleration = accelF;

}