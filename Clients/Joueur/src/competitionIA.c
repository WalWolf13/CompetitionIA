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

/*
    Converti une chaine de caractere correspondant à une adresse IPv4 en une adresse utilisable
*/
in_addr_t IPv4_address(char address[]){

    char chaine[4][4];
    int k = 0;
    for(int i = 0; i < 4; i++){
        char c = address[k];
        k++;
        int j = 0;
        while(c != '.' && c != '\0'){
            printf("%c", c);
            chaine[i][j] = c;
            j++;
            c = address[k];
            k++;
            if(j > 3){
                printf("Veuillez rentrer une adresse IPv4 correct\n");
                exit(-1);
            }
        }
        chaine[i][j] = '\0';
    }
    printf("\nL'adresse est %s.%s.%s.%s\n", chaine[0], chaine[1], chaine[2], chaine[3]);
    
    uint32_t adresse = 0;
    for(int i = 0; i < 4; i++){
        adresse <<= 8;
        adresse += atoi(chaine[i]);
    }
    printf("L'adresse : %d\n", adresse);
    return htonl(adresse);
}

/*
    Recupere les informations envoyées par le serveur durant la course.
    @param msgRec les informations envoyées par le serveur se trouveront dans cette structure.
    @return 0 la course continue.\n
    @return 1 la course est en pause. Si la course est en pause elle s'arrête.\n
    @return 2 la course est finie.
*/
int recuperationInformationCourse(msgServeurRec *msgRec, int nbDetecteur, int socket_fd){
    int etat = 0;
    for(int i = 0; i < nbDetecteur; i++){
        recv(socket_fd, &msgRec->detecteur[i], 2, 0);
    }
    recv(socket_fd, &msgRec->sens, 1, 0);
    recv(socket_fd, &msgRec->vitesse, 2, 0);
    recv(socket_fd, &msgRec->anglesRoues, 2, 0);
    recv(socket_fd, &etat, 1, 0);
    return etat;
}

/*
    Permet de se connecter au serveur de la competition
    @return socket de communication
*/
int connexion(char address[], char nomDuJoueur[], uint8_t nbDetecteur, uint16_t detecteur[]){
    //Verification des données rentrée
    if(nbDetecteur < 1 || nbDetecteur > 10){
        printf("Le nombre de detecteur doit être compris dans [|1, 10|]\n");
        exit(-1);
    }
    //Connexion au serveur
    struct sockaddr_in socketAddr;
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(25566);
    socketAddr.sin_addr.s_addr = IPv4_address(address);

    int socket_fd;
    socket_fd = socket(socketAddr.sin_family, SOCK_STREAM, 0);

    if(connect(socket_fd, (struct sockaddr *) &socketAddr, sizeof(socketAddr)) == -1){
        printf("Connection impossible...\n");
        exit(-1);
    }

    //Poigne de main
    uint8_t jou[3] = {'J', 'O', 'U'};
    send(socket_fd, jou, 3, 0);
    recv(socket_fd, jou, 3, 0);
    if(jou[0] != 'J' || jou[1] != 'O' || jou[2] != 'K'){
        printf("Le serveur ne correspond pas a un serveur pour la compétition IA\n");
        exit(-1);
    }
    //Envoi du nom du joueur
    uint8_t nbCaractereNomDuJoueur = strlen(nomDuJoueur);
    send(socket_fd, &nbCaractereNomDuJoueur, 1, 0);
    send(socket_fd, nomDuJoueur, nbCaractereNomDuJoueur, 0);

    //Envoi du nombre detecteur et de l'emplacement des detecteurs
    send(socket_fd, &nbDetecteur, 1, 0);
    for(int i = 0; i < nbDetecteur; i++){
        send(socket_fd, &detecteur[i], 2, 0);
    }

    //Verifiaction de la reponse du serveur
    uint8_t reponseServeur;
    recv(socket_fd, &reponseServeur, 1, 0);
    if(reponseServeur != 0){
        printf("Le serveur a invalidé la connexion\n");
        exit(-1);
    }

    return socket_fd;
}



double convertisseurDetecteur(uint16_t distInt){
    double rep = distInt;
    return rep*20/65535;
}