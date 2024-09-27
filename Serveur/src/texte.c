#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "erreur.h"
#include "map.h"
#include "texte.h"

#define pi (double) 3.141592653589793

chaineLexical init_chaineLexical(){return NULL;}
chaineLexical ajoute_chaineLexical(chaineLexical *chaine, objetLexical o, char* texte, int nombre, int ligne){
    chaineLexical c;
    if(*chaine == NULL){
        *chaine = malloc(sizeof(mailleLexical));
        c = *chaine;
    }
    else{
        c = *chaine;
        while(c->suite != NULL) c = c->suite;
        c-> suite = malloc(sizeof(mailleLexical));
        c = c->suite;
    }
    c->ligne = ligne;
    c->nombre = nombre;
    c->o = o;
    if(texte != NULL){
        int len = strlen(texte)+1;
        c->texte = malloc(sizeof(char)*(len));
        for(int i = 0; i < len; i++) c->texte[i] = texte[i];
    }
    else c->texte = NULL;
    c->suite = NULL;
    return c;
}

void free_chaineLexical(chaineLexical chaine){
    while(chaine != NULL){
        if(chaine->texte != NULL) free(chaine->texte);
        chaineLexical c = chaine;
        chaine = chaine->suite;
        free(c);
    }
}


/*
    Extrait les informations de la map
*/
void extraction_information_map(chaineLexical chaine, resultatGrammaire nb, Point* tabPoint, Ligne *tabLigne, Checkpoint *tabCheck, Emplacement* tabEmplacment, Arrivee* ligneDArrivee){
    chaineLexical explore = chaine;
    //Recuperation des coordonnes de chaque points
    while (explore->o != LEX_POINT){explore = explore->suite;}
    explore = explore->suite;
    for(int i = 0; i < nb.nbPoints; i++){
        tabPoint[i].x = explore->nombre;
        explore = explore->suite;
        tabPoint[i].y = explore->nombre;
        explore = explore->suite;
    }
    //Recuperation des coordonnes de chaque lignes
    while(explore->o != LEX_LIGNE) explore = explore->suite;
    explore = explore->suite;
    for(int i = 0; i < nb.nbLignes; i++){
        tabLigne[i].a = tabPoint[explore->nombre-1];
        explore = explore->suite;
        tabLigne[i].b = tabPoint[explore->nombre-1];
        explore = explore->suite;
    }
    //Recuperation des coordonnes de chaque checkpoints
    while(explore->o != LEX_CHECK) explore = explore->suite;
    explore = explore->suite;
    for(int i = 0; i < nb.nbCheckpoints; i++){
        tabCheck[i].a = tabPoint[explore->nombre-1];
        explore = explore->suite;
        tabCheck[i].b = tabPoint[explore->nombre-1];
        explore = explore->suite;
    }
    //Recuperation des coordonnes de chaque emplacements de departs
    while(explore->o != LEX_EMPLACEMENT) explore = explore->suite;
    explore = explore->suite;
    for(int i = 0; i < nb.nbEmplacements; i++){
        tabEmplacment[i].a = tabPoint[explore->nombre-1];
        explore = explore->suite;
        tabEmplacment[i].angle = explore->nombre*pi/180.;
        explore = explore->suite;
    }
    //Recuperation des coordonnes de la ligne d'arrivee
    while(explore->o != LEX_ARRIVEE) explore = explore->suite;
    explore = explore->suite;
    ligneDArrivee->a = tabPoint[explore->nombre-1];
    explore = explore->suite;
    ligneDArrivee->b = tabPoint[explore->nombre-1];
}

/*
    Verifie que le fichier en entree respecte l'analyse grammatical et semantique voulu
*/
resultatGrammaire analyse_grammatical(chaineLexical chaine){
    chaineLexical explore = chaine;
    resultatGrammaire r;
    r.nbPoints = 0;
    r.nbLignes = 0;
    r.nbCheckpoints = 0;
    r.nbEmplacements = 0;
    
    if(explore->o != LEX_NOM){
        AFFICHE_ERREUR_GRAMMAIRE(" Le fichier map doit commencer par le nom de la map !\n");
        exit(-1);
    }
    if(explore->ligne != 1){
        AFFICHE_ERREUR_GRAMMAIRE(" Le nom de la map doit etre sur la premiere ligne !\n");
        exit(-1);
    }
    explore = explore->suite;
    if(explore->o != LEX_NOMBRE){
        AFFICHE_ERREUR_GRAMMAIRE(" Le fichier map doit contenir le nombre de joueur maximum !\n");
        exit(-1);
    }
    if(explore->ligne != 2){
        AFFICHE_ERREUR_GRAMMAIRE(" Le nombre maximum de joueur doit se trouver sur la deuxieme ligne !\n");
        exit(-1);
    }
    int nbJoueur = explore->nombre;//Verification semantique
    int ligne = 3;
    explore = explore->suite;
    if(explore->o != LEX_POINT){
        AFFICHE_ERREUR_GRAMMAIRE(" Le fichier doit contenir apres le nombre de joueur une liste de coordonnee de points qui commence par \"points\"\n");
        exit(-1);
    }
    explore = explore->suite;
    while(explore->o != LEX_LIGNE){
        if(explore->o != LEX_NOMBRE){
            AFFICHE_ERREUR_GRAMMAIRE(" ");
            printf("(ligne %d) On doit retrouver deux nombres ecrit comme suit :\n\tXXX YYY\nOu le mot-cle \"lignes\"\n", explore->ligne);
            exit(-1);
        }
        ligne = explore->ligne;
        r.nbPoints++;
        explore = explore->suite;
        if((explore->o != LEX_NOMBRE) || (explore->ligne != ligne) ){
            AFFICHE_ERREUR_GRAMMAIRE(" ");
            printf("(ligne %d) On doit retrouver deux nombres ecrit comme suit sur la meme ligne :\n\tXXX YYY\n", ligne);
            exit(-1);
        }
        explore = explore->suite;
        ligne++;
    }
    explore = explore->suite;
    ligne++;
    //Verification grammatical et semantique de la partie "lignes"
    while(explore->o != LEX_CHECK){
        if(explore->o != LEX_NOMBRE){
            AFFICHE_ERREUR_GRAMMAIRE(" ");
            printf("(ligne %d) On doit retrouver deux nombres ecrit comme suit :\n\tXXX YYY\nOu le mot-cle \"lignes\"\n", explore->ligne);
            exit(-1);
        }
        ligne = explore->ligne;
        r.nbLignes++;
        //Verfication semantique> On verifie que le point existe
        if(explore->nombre > r.nbPoints){
            AFFICHE_ERREUR_SEMANTIQUE(ligne, "Le premier point de cette ligne n'existe pas\n");
            exit(-1);
        }

        explore = explore->suite;
        if((explore->o != LEX_NOMBRE) || (explore->ligne != ligne) ){
            AFFICHE_ERREUR_GRAMMAIRE(" ");
            printf("(ligne %d) On doit retrouver deux nombres ecrit comme suit sur la meme ligne :\n\tXXX YYY\n", ligne);
            exit(-1);
        }
        //Verfication semantique> On verifie que le point existe
        if(explore->nombre > r.nbPoints){
            AFFICHE_ERREUR_SEMANTIQUE(ligne, "Le second point de cette ligne n'existe pas\n");
            exit(-1);
        }
        explore = explore->suite;

        ligne++;
    }
    explore = explore->suite;
    ligne++;
    //Verification grammatical et semantique de la partie "checkpoints"
    while(explore->o != LEX_EMPLACEMENT){
        if(explore->o != LEX_NOMBRE){
            AFFICHE_ERREUR_GRAMMAIRE(" ");
            printf("(ligne %d) On doit retrouver deux nombres ecrit comme suit :\n\tXXX YYY\nOu le mot-cle \"lignes\"\n", explore->ligne);
            exit(-1);
        }
        ligne = explore->ligne;
        r.nbCheckpoints++;

        //Verfication semantique> On verifie que le point existe
        if(explore->nombre > r.nbPoints){
            AFFICHE_ERREUR_SEMANTIQUE(ligne, "Le premier point de cette ligne n'existe pas\n");
            exit(-1);
        }

        explore = explore->suite;
        if((explore->o != LEX_NOMBRE) || (explore->ligne != ligne) ){
            AFFICHE_ERREUR_GRAMMAIRE(" ");
            printf("(ligne %d) On doit retrouver deux nombres ecrit comme suit sur la meme ligne :\n\tXXX YYY\n", ligne);
            exit(-1);
        }

        //Verfication semantique> On verifie que le point existe
        if(explore->nombre > r.nbPoints){
            AFFICHE_ERREUR_SEMANTIQUE(ligne, "Le second point de cette ligne n'existe pas\n");
            exit(-1);
        }

        explore = explore->suite;
        ligne++;
    }
    explore = explore->suite;
    ligne++;
    //Verification grammatical et semantique de la partie "emplacement"
    while(explore->o != LEX_ARRIVEE){
        if(explore->o != LEX_NOMBRE){
            AFFICHE_ERREUR_GRAMMAIRE(" ");
            printf("(ligne %d) On doit retrouver deux nombres ecrit comme suit :\n\tXXX YYY\nOu le mot-cle \"lignes\"\n", explore->ligne);
            exit(-1);
        }
        ligne = explore->ligne;
        r.nbEmplacements++;

        //Verfication semantique> On verifie que le point existe
        if(explore->nombre > r.nbPoints){
            AFFICHE_ERREUR_SEMANTIQUE(ligne, "Le point de cette ligne n'existe pas\n");
            exit(-1);
        }

        explore = explore->suite;
        if((explore->o != LEX_NOMBRE) || (explore->ligne != ligne) ){
            AFFICHE_ERREUR_GRAMMAIRE(" ");
            printf("(ligne %d) On doit retrouver deux nombres ecrit comme suit sur la meme ligne :\n\tXXX YYY\n", ligne);
            exit(-1);
        }
        explore = explore->suite;
        ligne++;
    }
    //Verification grammatical et semantique de la partie "arrivee"
    ligne = explore->ligne;
    explore = explore->suite;
    if(explore->o != LEX_NOMBRE){
        AFFICHE_ERREUR_GRAMMAIRE(" ");
        printf("(ligne %d) la ligne d'arriver dans le fichier map se decrit comme suit : \"arrivee X Y\" ou X et Y sont des nombres\n", ligne);
        exit(-1);
    }
    if(explore->ligne != ligne){
        AFFICHE_ERREUR_GRAMMAIRE(" ");
        printf("(ligne %d) les points decrivant l'arrivee doivent etre sur la meme ligne que le mot arrivee\n", ligne);
        exit(-1);
    }
    //Verfication semantique> On verifie que le point existe
    if(explore->nombre > r.nbPoints){
        AFFICHE_ERREUR_SEMANTIQUE(ligne, "Le point de cette ligne n'existe pas\n");
        exit(-1);
    }

    explore = explore->suite;
    if(explore->o != LEX_NOMBRE){
        AFFICHE_ERREUR_GRAMMAIRE(" ");
        printf("(ligne %d) la ligne d'arriver dans le fichier map se decrit comme suit : \"arrivee X Y\" ou X et Y sont des nombres\n", ligne);
        exit(-1);
    }
    if(explore->ligne != ligne){
        AFFICHE_ERREUR_GRAMMAIRE(" ");
        printf("(ligne %d) les points decrivant l'arrivee doivent etre sur la meme ligne que le mot arrivee\n", ligne);
        exit(-1);
    }
    //Verfication semantique> On verifie que le point existe
    if(explore->nombre > r.nbPoints){
        AFFICHE_ERREUR_SEMANTIQUE(ligne, "Le second point de cette ligne n'existe pas\n");
        exit(-1);
    }

    //Verfication semantique> On verifie que le nombre de joueur correspond au nombre indiquer
    if(r.nbEmplacements < nbJoueur){
        AFFICHE_ERREUR_SEMANTIQUE(2, "Le nombre de joueur ne correspond au nombre d'emplacement\n");
        exit(-1);
    }
    
    return r;
}


chaineLexical lexer(FILE* fichier){
    bool premiereIte = false;
    chaineLexical chaine = init_chaineLexical();
    chaineLexical derniereMaille = chaine;
    int ligne = 1;
    char caract = fgetc(fichier);
    while(caract != EOF){
        //cas d'un nom dans la grammaire decrite dans la documentation
        if((caract >= 'a' && caract <= 'z') || (caract >= 'A' && caract <= 'Z') || (caract == '_')){
            int len = 10;
            int i = 0;
            char* texte = malloc(sizeof(char)*len);
            while ((caract >= 'a' && caract <= 'z') || (caract >= 'A' && caract <= 'Z') || (caract >= '0' && caract <= '9') || (caract == '_'))
            {
                texte[i] = caract;
                i++;
                if(i >= len){
                    len += 10;
                    texte = realloc(texte, len*sizeof(char));
                }
                caract = fgetc(fichier);
            }
            texte[i] = '\0';
            if(strcmp(texte, "points") == 0) derniereMaille = ajoute_chaineLexical(&derniereMaille, LEX_POINT, NULL, 0, ligne);
            else if(strcmp(texte, "lignes") == 0) derniereMaille = ajoute_chaineLexical(&derniereMaille, LEX_LIGNE, NULL, 0, ligne);
            else if(strcmp(texte, "checkpoints") == 0) derniereMaille = ajoute_chaineLexical(&derniereMaille, LEX_CHECK, NULL, 0, ligne);
            else if(strcmp(texte, "emplacements") == 0) derniereMaille = ajoute_chaineLexical(&derniereMaille, LEX_EMPLACEMENT, NULL, 0, ligne);
            else if(strcmp(texte, "arrivee") == 0) derniereMaille = ajoute_chaineLexical(&derniereMaille, LEX_ARRIVEE, NULL, 0, ligne);
            else derniereMaille = ajoute_chaineLexical(&derniereMaille, LEX_NOM, texte, 0, ligne);
            free(texte);
        }
        //Cas d'un nombre
        else if(caract >= '0' && caract <= '9'){
            int resultat = 0;
            while(caract >= '0' && caract <= '9'){
                resultat = resultat*10;
                resultat += caract-'0';
                caract = fgetc(fichier);
            }
            derniereMaille = ajoute_chaineLexical(&derniereMaille, LEX_NOMBRE, NULL, resultat, ligne);
        }
        //Saut de ligne
        else if(caract == '\n'){
            ligne += 1;
            caract = fgetc(fichier);
        }
        else if(caract == ' ' || caract == '\r') caract = fgetc(fichier);
        else{
            //caract = fgetc(fichier);
            AFFICHE_ERREUR_LEXIQUE(" caractere inconnu dans le fichier map\n");
            printf("\tLe caractere est '%c'\n", caract);
            exit(-1);
        }

        if(!premiereIte && derniereMaille != NULL){
            chaine = derniereMaille;
            premiereIte = true;
        }
    }
    return chaine;
}