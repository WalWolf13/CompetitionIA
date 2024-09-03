#ifndef __ERREUR_H__
#define __ERREUR_H__

#define AFFICHE_ERREUR(texte) printf("\033[38;2;255;0;0m");printf("ERREUR :");printf("\033[m");printf(texte)
#define AFFICHE_ERREUR_LEXIQUE(texte) printf("\033[38;2;255;0;0m");printf("ERREUR LEXICAL :");printf("\033[m");printf(texte)
#define AFFICHE_ERREUR_GRAMMAIRE(texte) printf("\033[38;2;255;0;0m");printf("ERREUR GRAMMATICAL :");printf("\033[m");printf(texte)
#define AFFICHE_ERREUR_SEMANTIQUE(ligne, texte) printf("\033[38;2;255;0;0m");printf("ERREUR SEMANTIQUE :");printf("\033[m(ligne %d) ", ligne);printf(texte)

#endif