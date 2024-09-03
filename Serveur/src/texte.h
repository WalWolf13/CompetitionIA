#ifndef __TEXTE_H__
#define __TEXTE_H__

#include <stdio.h>

typedef enum objetLexical{
    LEX_NOM, LEX_NOMBRE, LEX_POINT, LEX_LIGNE, LEX_CHECK, LEX_EMPLACEMENT, LEX_ARRIVEE
} objetLexical;

typedef struct mailleLexical{
    objetLexical o;
    char*  texte;
    int nombre;
    int ligne;
    struct mailleLexical* suite;
}mailleLexical;

typedef mailleLexical* chaineLexical;

typedef struct resultatGrammaire{
    int nbPoints;
    int nbLignes;
    int nbCheckpoints;
    int nbEmplacements;
}resultatGrammaire;

chaineLexical lexer(FILE* fichier);
resultatGrammaire analyseGrammatical(chaineLexical chaine);

chaineLexical initChaineLexical();
chaineLexical addChaineLexical(chaineLexical *chaine, objetLexical o, char* texte, int nombre, int ligne);
void extractionInformationMap(chaineLexical chaine, resultatGrammaire nb, Point* tabPoint, Ligne *tabLigne, Checkpoint *tabCheck, Emplacement* tabEmplacment, Arrivee* ligneDArrivee);
void freeChaineLexical(chaineLexical chaine);

#endif