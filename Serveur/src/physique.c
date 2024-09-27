#include <unistd.h>
#include <signal.h>
#include <math.h>

#include "physique.h"

void collision_voiture_mur(Voiture* resultat, infoBaseMap *infoMap, Ligne* tabLignes, Voiture* trajectoireSansCollision){
    Rectangle voitureGeo;
    voitureGeo = recupere_point_Voiture_double(trajectoireSansCollision);
    bool collision = false;
    for(int i = 0; i < infoMap->nbLignes && collision == false; i++){
        if(intersection_rectangle_ligne(voitureGeo, tabLignes[i]))collision = true;
    }
    if(collision) resultat->vitesse = 0;
    else *resultat = *trajectoireSansCollision;

}

void avancer_voiture(Voiture *voiture, infoBaseMap *infoMap, Ligne* tabLignes){
    Voiture trajectoireSansCollision;
    trajectoireSansCollision = *voiture;

    //Calcule de la trajectoire
    calcule_trajectoire(&trajectoireSansCollision);
    //Calcule des colisions
    collision_voiture_mur(voiture, infoMap, tabLignes, &trajectoireSansCollision);
    

    //Mesure des detecteurs
    mesure_detecteurs(voiture, tabLignes, infoMap->nbLignes);
            
    //Envoi du signal au client
    kill(voiture->pid, SIGUSR1);       
}

void avancer_monde(Voiture* joueur, int nbJoueur, infoBaseMap *infoMap, Ligne* tabLignes){
    //Application de la physique sur chaque vehicule
        for(int i = 0; i < nbJoueur; i++){
            if(!joueur[i].fini) avancer_voiture(&joueur[i], infoMap, tabLignes);
        }
}