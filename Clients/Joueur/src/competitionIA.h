#ifndef __COMPETITION_IA__
#define __COMPETITION_IA__

    #include <stdint.h>
    #define pi (double) 3.141592653589793
    typedef struct detect
    {
        uint8_t nbDetecteur;
        double detecteur[10];
        uint16_t detecteurMsg[10];
    } detect;
    

    typedef struct msgServeurEnv
    {
        uint8_t sens;
        int16_t acceleration;
        uint16_t angleRoues;
    } msgServeurEnv;

    typedef struct msgServeurRec
    {
        uint16_t detecteur[10];
        uint8_t sens;
        int16_t vitesse;
        uint16_t anglesRoues;
    } msgServeurRec;

    typedef enum sensVehicule{
        MARCHE_AVANT, MARCHE_ARRIERE
    } sensVehicule;

    typedef struct entreeControleur
    {
        double detecteur[10];
        sensVehicule sens;
        double vitesse;
        double anglesRoues;  //Bornee entre -30° et 30°
    } entreeControleur;

    typedef struct sortieControleur
    {
        sensVehicule sens;
        int acceleration;
        double angleRoues; //Bornee entre -30° et 30°
    } sortieControleur;
    

    void calculMessageDetecteur(detect *detecteurs);

    int connexion(char address[], char nomDuJoueur[], uint8_t nbDetecteur, uint16_t detecteur[]);
    int recuperationInformationCourse(msgServeurRec *msgRec, int nbDetecteur, int socket_fd);

    entreeControleur convertisseurEntree(msgServeurRec *msg, int nbDetecteur);
    msgServeurEnv convertisseurSortie(sortieControleur *msg);

#endif