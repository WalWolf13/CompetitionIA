#include <math.h>
#include <stdbool.h>
#include "geometrie.h"


//Calcule la distance entre deux points
double distance_point(Point a, Point b){
    return sqrt((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y));
}

//Projet orthogonalement le point A sur la droite (BC)
Point projete_orthogonale(Point a, Ligne bc){
    double k = -((bc.b.x - bc.a.x)*(bc.a.x-a.x)+(bc.b.y-bc.a.y)*(bc.a.y-a.y))/((bc.b.x-bc.a.x)*(bc.b.x-bc.a.x)+(bc.b.y-bc.a.y)*(bc.b.y-bc.a.y));
    Point resultat;
    resultat.x = bc.a.x + k*(bc.b.x-bc.a.x);
    resultat.y = bc.a.y + k*(bc.b.y-bc.a.y);
    return resultat;
}

//Indique si le point C appartient à la demi droite  [AB)
bool appartient_a_une_demi_droite(Ligne ab, Point c){
    double k1 = (c.x-ab.a.x)/(ab.b.x-ab.a.x);
    double k2 = (c.y-ab.a.y)/(ab.b.y-ab.a.y);
    if(fabs(k1-k2) < 0.01 && k1 >= 0) return true;
    return false;
}

//Indique si le point C appartient au segment [AB]
bool appartient_a_un_segment(Ligne ab, Point c){
    double k1 = (c.x-ab.a.x)/(ab.b.x-ab.a.x);
    double k2 = (c.y-ab.a.y)/(ab.b.y-ab.a.y);
    if(fabs(k1-k2) < 0.01 && k1 >= 0 && k1 <= 1) return true;
    return false;
}

//Calcule l'angle ABC
double angleABC(Point a, Point b, Point c){
    Ligne ba;
    ba.a = b;
    ba.b = a;
    Point p = projete_orthogonale(c, ba);
    double alpha = acos(distance_point(b, p)/distance_point(b, c));
    if(appartient_a_une_demi_droite(ba, p)) return alpha;
    return (pi-alpha);
}

//Calcule la position du point sur les bords du rectangle ABCD à partir de son centre selon l'angle dans le sens trigonométrique 
Point point_sur_rectangle_selon_angle(Rectangle abcd, double angle){
    double l = distance_point(abcd.a, abcd.b)/2;
    double L = distance_point(abcd.a, abcd.d)/2;
    double angleAVE = atan(L/l);
    double angleBVE = pi-angleAVE;
    Point p;
    if(angle <= angleAVE && angle >= -angleAVE){
        double k = 0.5-(l*tan(angle))/(2*L);
        p.x = abcd.a.x + k * (abcd.d.x-abcd.a.x);
        p.y = abcd.a.y + k * (abcd.d.y-abcd.a.y);
    }
    else if(angle >= angleAVE && angle <= angleBVE){
        double k = 0.5-L/(2*l*tan(angle));
        p.x = abcd.a.x + k * (abcd.b.x-abcd.a.x);
        p.y = abcd.a.y + k * (abcd.b.y-abcd.a.y);

    }
    else if(angle >= -angleBVE && angle <= -angleAVE){
        Point c;
        c.x = abcd.d.x + abcd.b.x - abcd.a.x;
        c.y = abcd.d.y + abcd.b.y - abcd.a.y;
        double k = 0.5+(L)/(2*l*tan(angle));
        p.x = abcd.d.x + k*(c.x-abcd.d.x);
        p.y = abcd.d.y + k*(c.y-abcd.d.y);
    }
    else{
        Point c;
        c.x = abcd.d.x + abcd.b.x - abcd.a.x;
        c.y = abcd.d.y + abcd.b.y - abcd.a.y;
        double k = 0.5+(l*tan(angle))/(2*L);
        p.x = abcd.b.x + k*(c.x-abcd.b.x);
        p.y = abcd.b.y + k*(c.y-abcd.b.y);
    }
    return p;
}

//Indique et calcule si il existe un point en commun entre la demi droite [AB) et le segment [CD]
ReponsePoint point_intersection_entre_demi_droite_et_degment(DemiDroite ab, Ligne cd){
    ReponsePoint rep;
    double xBA = ab.b.x - ab.a.x;
    double yCA = cd.a.y - ab.a.y;
    double xCA = cd.a.x - ab.a.x;
    double xDC = cd.b.x - cd.a.x;
    double yBA = ab.b.y - ab.a.y;
    double yDC = cd.b.y - cd.a.y;

    double yAC = ab.a.y - cd.a.y;
    double xAC = ab.a.x - cd.a.x;
    double l = (xBA*yCA-xCA*yBA)/(xDC*yBA-xBA*yDC);
    double k = (xDC*yAC-xAC*yDC)/(xBA*yDC-xDC*yBA);
    if(k < 0 || l > 1 || l < 0){
        rep.existe = false;
    }
    else{
        rep.existe = true;
        rep.p.x = cd.a.x + l*(cd.b.x - cd.a.x);
        rep.p.y = cd.a.y + l*(cd.b.y - cd.a.y);
    }
    return rep;
}

//Indique si le point P est à l'interieur ou sur les bords du rectangle [ABCD]
bool appartient_a_un_rectangle(Rectangle abcd, Point p){
    double xDA = abcd.d.x-abcd.a.x;
    double yPA = p.y-abcd.a.y;
    double xPA = p.x-abcd.a.x;
    double yDA = abcd.d.y-abcd.a.y;
    double yBA = abcd.b.y-abcd.a.y;
    double xBA = abcd.b.x-abcd.a.x;
    double k = (xDA*yPA-xPA*yDA)/(xDA*yBA-xBA*yDA);
    double l = (xBA*yPA-xPA*yBA)/(xBA*yDA-xDA*yBA);
    if(k >= 0 && k <= 1 && l >= 0 && l <= 1) return true;
    return false;
}

//Retourne vrai si l'intersection entre les deux lignes est non vide
bool intersection_deux_lignes(Ligne ab, Ligne cd){
    double xBA = ab.b.x - ab.a.x;
    double yCA = cd.a.y - ab.a.y;
    double xCA = cd.a.x - ab.a.x;
    double xDC = cd.b.x - cd.a.x;
    double yBA = ab.b.y - ab.a.y;
    double yDC = cd.b.y - cd.a.y;

    double yAC = -yCA;
    double xAC = -xCA;
    double l = (xBA*yCA-xCA*yBA)/(xDC*yBA-xBA*yDC);
    double k = (xDC*yAC-xAC*yDC)/(xBA*yDC-xDC*yBA);
    if(l >= 0 && l <= 1 && k >= 0 && k <= 1) return true;
    return false;
}

//Retourne vrai si l'intersection entre le rectangle et la ligne est non vide
bool intersection_rectangle_ligne(Rectangle abcd, Ligne pq){
    
    Point c;
    c.x = abcd.d.x + abcd.b.x - abcd.a.x;
    c.y = abcd.d.y + abcd.b.y - abcd.a.y;
    Ligne ab, bc, cd, da;
    ab.a = abcd.a;
    ab.b = abcd.b;
    bc.a = abcd.b;
    bc.b = c;
    cd.a = c;
    cd.b = abcd.d;
    da.a = abcd.d;
    da.b = abcd.a;
    return intersection_deux_lignes(ab, pq) || intersection_deux_lignes(bc, pq) || intersection_deux_lignes(cd, pq) || intersection_deux_lignes(da, pq);
}