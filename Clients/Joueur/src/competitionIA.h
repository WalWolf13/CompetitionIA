#ifndef __COMPETITION_IA__
#define __COMPETITION_IA__

    #include <stdint.h>

    typedef struct detect
    {
        uint8_t nbDetecteur;
        uint16_t detecteur[10];
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
        uint16_t vitesse;
        uint16_t anglesRoues;
    } msgServeurRec;
    


    int connexion(char address[], char nomDuJoueur[], uint8_t nbDetecteur, uint16_t detecteur[]);
    int recuperationInformationCourse(msgServeurRec *msgRec, int nbDetecteur, int socket_fd);


    double convertisseurDetecteur(uint16_t distInt);

#endif