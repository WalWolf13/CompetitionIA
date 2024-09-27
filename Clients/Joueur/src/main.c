#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "competitionIA.h"
#include "codeAModifier.h"


int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Veuillez mettre un argument qui est l'adresse IPv4 du serveur\n");
        exit(-1);
    }
    detect detecteur = initialisation();
    int socket_fd = connexion(argv[1], __COMPETITION_IA_NOM_DU_JOUEUR__, detecteur.nbDetecteur, detecteur.detecteur);
    uint8_t msgServ = 0;
    printf("En attente de demarrage de la course !\n");
    recv(socket_fd, &msgServ, 1, 0);
    if(msgServ != 1){
        printf("La course n'as pas pus se lancer.\n");
        exit(-1);
    }
    printf("La course demarre !\n");
    int etat = 0;//correspond à l'etat de la course
    msgServeurRec msgRec;
    msgServeurEnv msgEnv;
    while(etat == 0){
        etat = recuperationInformationCourse(&msgRec, detecteur.nbDetecteur, socket_fd);
        controle(&msgRec, &msgEnv);
        //send(socket_fd, &msgEnv, 5, 0);
        send(socket_fd, &msgEnv.sens, 1, 0);
        send(socket_fd, &msgEnv.acceleration, 2, 0);
        send(socket_fd, &msgEnv.angleRoues, 2, 0);
        if(etat == 1){
            recv(socket_fd, &etat, 1, 0);
        }
    }

    return 0;
}