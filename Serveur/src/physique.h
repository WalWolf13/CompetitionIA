#ifndef __PHYSIQUE_H__
#define __PHYSIQUE_H__

    #define TPS_ECHANTILLONNAGE 5 //ms

    #include "voiture.h"
    #include "map.h"

    void avancer_monde(Voiture* joueur, int nbJoueur, infoBaseMap *infoMap, Ligne* tabLignes);

#endif