#ifndef __MAP_H__
#define __MAP_H__

    /*
    Dans ce fichier nous allons retrouver tous ce qui concerne la descrition de la map
    */

   #include <stdint.h>
   #include "geometrie.h"

    #define MAP_KEY_NAME "map"
    #define PATH_FOLDER "data/map/"

    typedef struct infoBaseMap
    {
        uint16_t nbPoints;
        uint16_t nbLignes;
        uint16_t nbCheckpoints;
        uint16_t nbEmplacements;
    }infoBaseMap;

    typedef struct Checkpoint{
        Point a, b;
    }Checkpoint;

    typedef struct Emplacement{
        Point a;
        double angle;
    }Emplacement;

    typedef struct Arrivee{
        Point a, b;
    }Arrivee;


int chargeFichierMap(char *fileName);

#endif