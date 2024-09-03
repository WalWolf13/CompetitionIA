#include <stdlib.h>
#include <math.h>
#include "voiture.h"


Voiture* initVoiture(double longueur, double largeur, double omega, double x, double y){
    Voiture* v = (Voiture *) malloc(sizeof(Voiture));
    v->largeur = largeur;
    v->longueur = longueur;
    v->omega = omega;
    v->x = x;
    v->y = y;

    v->acceleration = 0;
    v->vitesse = 0;
    v->marche_av_ar = VOITURE_M_AVANT;
    v->angleRoue = 0;

    return v;
}

PointsVoiture *recupePointVoiture(Voiture *v){
    PointsVoiture *p = (PointsVoiture *) malloc(sizeof(PointsVoiture));
    double L = v->largeur/2;
    double l = v->longueur/2;
    p->xa = (cos(v->omega) * l - sin(v->omega) * L + v->x)*1000; 
    p->ya = (cos(v->omega) * L + sin(v->omega) * l + v->y)*1000;
    
    p->xb = (-cos(v->omega) * l - sin(v->omega) * L + v->x)*1000; 
    p->yb = (cos(v->omega) * L - sin(v->omega) * l + v->y)*1000;
    
    p->xc = (-cos(v->omega) * l + sin(v->omega) * L + v->x)*1000; 
    p->yc = (-cos(v->omega) * L - sin(v->omega) * l + v->y)*1000;
    
    p->xd = (cos(v->omega) * l + sin(v->omega) * L + v->x)*1000; 
    p->yd = (-cos(v->omega) * L + sin(v->omega) * l + v->y)*1000;
    return p;
}

Rectangle recupePointVoitureDouble(Voiture *v){
    Rectangle r;
    double L = v->largeur/2;
    double l = v->longueur/2;
    r.a.x = (cos(v->omega) * l - sin(v->omega) * L + v->x); 
    r.a.y = (cos(v->omega) * L + sin(v->omega) * l + v->y);
    
    r.b.x = (-cos(v->omega) * l - sin(v->omega) * L + v->x); 
    r.b.y = (cos(v->omega) * L - sin(v->omega) * l + v->y);
    
    r.d.x = (cos(v->omega) * l + sin(v->omega) * L + v->x); 
    r.d.y = (-cos(v->omega) * L + sin(v->omega) * l + v->y);
    return r;
}

double mesureDetecteur(Rectangle abcd, Point v, double angle, Ligne *mur, int nbMur){
    Point p = pointSurRectangleSelonAngle(abcd, angle);
    DemiDroite vp;
    vp.a = v;
    vp.b = p;
    double distance = DISTANCE_MAX_DETECTEUR_VOITURE;
    for(int i = 0; i < nbMur; i++){
        ReponsePoint rep = pointIntersectionEntreDemiDroiteEtSegment(vp, mur[i]);
        if(rep.existe){
            double distanceActuel = distancePoint(p, rep.p);
            if(distanceActuel < distance) distance = distanceActuel;
        }
    }
    return distance;
}

void  mesureDetecteurs(Voiture *v, Ligne *mur, int nbMur){
    Rectangle abcd = recupePointVoitureDouble(v);
    Point centre;
    centre.x = v->x;
    centre.y = v->y;
    for(int i = 0; i < v->nbDetecteur; i++){
        v->distDetecteur[i] = mesureDetecteur(abcd, centre, v->detecteur[i], mur, nbMur);
        //printf("Detecteur[%d] : %f m\n", i, v->distDetecteur[i]);
    }
}

void freeVoiture(Voiture *v){
    free(v);
}