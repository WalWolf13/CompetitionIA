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

    double distancePoint(Point a, Point b);
    Point projeteOrthogonale(Point a, Ligne bc);
    bool interRectangleLigne(Rectangle abcd, Ligne pq);
    Point pointSurRectangleSelonAngle(Rectangle abcd, double angle);
    ReponsePoint pointIntersectionEntreDemiDroiteEtSegment(DemiDroite ab, Ligne cd);

#endif