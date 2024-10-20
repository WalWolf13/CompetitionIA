#include <stdlib.h>
#include <math.h>
#include "voiture.h"
#include "physique.h"


Voiture* init_Voiture(double longueur, double largeur, double omega, double x, double y){
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

    v->numeroDeCheckpoint = 0;
    v->fini = false;
    v->tempDeFin = 0;

    return v;
}

PointsVoiture *recupere_point_Voiture(Voiture *v){
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

Rectangle recupere_point_Voiture_double(Voiture *v){
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

double mesure_detecteur(Rectangle abcd, Point v, double angle, Ligne *mur, int nbMur){
    Point p = point_sur_rectangle_selon_angle(abcd, -angle);
    DemiDroite vp;
    vp.a = v;
    vp.b = p;
    double distance = DISTANCE_MAX_DETECTEUR_VOITURE;
    for(int i = 0; i < nbMur; i++){
        ReponsePoint rep = point_intersection_entre_demi_droite_et_degment(vp, mur[i]);
        if(rep.existe){
            double distanceActuel = distance_point(p, rep.p);
            if(distanceActuel < distance) distance = distanceActuel;
        }
    }
    return distance;
}

void  mesure_detecteurs(Voiture *v, Ligne *mur, int nbMur){
    Rectangle abcd = recupere_point_Voiture_double(v);
    Point centre;
    centre.x = v->x;
    centre.y = v->y;
    for(int i = 0; i < v->nbDetecteur; i++){
        v->distDetecteur[i] = mesure_detecteur(abcd, centre, v->detecteur[i], mur, nbMur);
        //printf("Detecteur[%d] : %f m\n", i, v->distDetecteur[i]);
    }
}


double accel_vers_force_marche(int16_t accel){
    //double commande = accel;
    double f = 0;
    if(accel > 0){
        f = accel*(((double) 8213.)/((double) 32767.));
    }
    else{
        f = accel*(((double) 12735.)/((double) 32768.));
    }
    return f;
}

double calcule_vitesse(double v, int16_t accel, Voiture_Marche sens){
    double p_0 = 0.99942;
    double a = 6.664748214e-6;
    double vitesseFinal = p_0*v;
    if(sens == VOITURE_M_AVANT && vitesseFinal > -0.5){
        double force = accel_vers_force_marche(accel);
        vitesseFinal = p_0*v + a*force;
        if(vitesseFinal > 100.) vitesseFinal = 100.;
        else if(vitesseFinal < 0.) vitesseFinal = 0.;
    }
    else if(sens == VOITURE_M_ARRIERE && vitesseFinal < 0.5){
        double force = accel_vers_force_marche(accel);
        vitesseFinal = p_0*v - a*force;
        if(vitesseFinal > 0.) vitesseFinal = 0.;
        else if(vitesseFinal < -2.7) vitesseFinal = -2.7;
    }
    return vitesseFinal;
}

void calcule_trajectoire(Voiture *v){
    v->vitesse = calcule_vitesse(v->vitesse, v->acceleration, v->marche_av_ar);

    double x;
    double y;
    if(v->angleRoue != 0){
        double R = EMPATTEMENT / sin(v->angleRoue);
        double alpha = v->vitesse*(0.005)*sin(v->angleRoue)/(EMPATTEMENT);
        x = R*(1-cos(alpha));
        y = R*sin(alpha);
    }
    else{
        x = 0;
        y = v->vitesse*(0.005);
    }

    v->x += x*sin(v->omega) + y*cos(v->omega);
    v->y += y*sin(v->omega) - x*cos(v->omega);
    if(y != 0){
        v->omega = v->omega - 2*atan(x/y);
    }
}


void free_Voiture(Voiture *v){
    free(v);
}