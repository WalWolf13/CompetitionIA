#include "competitionIA.h"

#include <stdio.h>

detect initialisation(){
    detect detecteur;
    detecteur.nbDetecteur = 3; //Choisir le nombre de detecteur voulu entre 1 et 10 inclu
    //Configuration de chaque detecteur :
    detecteur.detecteur[0] = 0;
    detecteur.detecteur[1] = 36940;
    detecteur.detecteur[2] = 28596;

    return detecteur;
}

int compteur= 0;


//Contrôle le comportement de la voiture selon les valeurs reçues
void controle(msgServeurRec *informationRecu, msgServeurEnv *informationAEnvoyer){
    //printf("Vitesse recu : %d cm/s\n", informationRecu->vitesse);
    
    // printf("Detecteur :\n");
    // for(int i = 0; i < 3; i++){
    //     printf("\t%d : %f\n", i, convertisseurDetecteur(informationRecu->detecteur[i]));
    // }

    double vitesseConsigne = 1000;// m/s
    double vitesse = ((double)informationRecu->vitesse) / 100.;
    //if(convertisseurDetecteur(informationRecu->detecteur[0]) > 19) vitesseConsigne = 3;

    double accelF = (vitesseConsigne - vitesse)*750;
    int16_t accel = 0;
    if(accelF > 32767) accel = 32767;
    else if (accelF < -32767) accel = -32767;
    else accel = accelF;
    //accel = 32767;
    printf("vitesse : %f\n", vitesse);
    //int8_t accel = 1;
    // if(compteur >= 5) accel = 0;
    //if(convertisseurDetecteur(informationRecu->detecteur[0])< 3.0) accel = -127;
    //if(compteur > 300 && compteur < 200) printf("accel : %d\n", accel);

    double esp = convertisseurDetecteur(informationRecu->detecteur[1])-convertisseurDetecteur(informationRecu->detecteur[2]);
    if(esp > 0) {
        informationAEnvoyer->angleRoues = 0;//49152;
        //printf("Gauche\n");
    }
    else{
        informationAEnvoyer->angleRoues = 65535;
        //printf("Droite\n");
    } 
    //else informationAEnvoyer->angleRoues = 32768;

    informationAEnvoyer->acceleration = accel;
    //informationAEnvoyer->angleRoues = 0;//49152;
    informationAEnvoyer->sens = 0;


    compteur++;
}