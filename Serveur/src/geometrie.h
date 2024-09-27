#ifndef __GEOMETRIE_H__
#define __GEOMETRIE_H__
    #include <stdbool.h>
    #define pi (double) 3.141592653589793
    
    typedef struct Point{
        double x, y;
    }Point;

    typedef struct Ligne{
        Point a, b;
    }Ligne;

    typedef struct DemiDroite{
        Point a, b;
    }DemiDroite;

    typedef struct Rectangle{
        Point a, b, d;
    }Rectangle;

    typedef struct ReponsePoint{
        bool existe;
        Point p;
    }ReponsePoint;

    double distance_point(Point a, Point b);
    Point projete_orthogonale(Point a, Ligne bc);
    bool intersection_rectangle_ligne(Rectangle abcd, Ligne pq);
    Point point_sur_rectangle_selon_angle(Rectangle abcd, double angle);
    ReponsePoint point_intersection_entre_demi_droite_et_degment(DemiDroite ab, Ligne cd);

#endif