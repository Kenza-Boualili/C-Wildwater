#ifndef AVL_RECHERCHE_H
#define AVL_RECHERCHE_H
#include "structures.h"


NoeudAVLRecherche* creerNoeudAVLRecherche(char* identifiant, NoeudDistribution* noeud);
void libererAVLRecherche(NoeudAVLRecherche* racine);
int hauteurAVLRecherche(NoeudAVLRecherche* noeud);
NoeudAVLRecherche* equilibrerAVLRecherche(NoeudAVLRecherche* racine);
NoeudAVLRecherche* rotationGaucheRecherche(NoeudAVLRecherche* x);
NoeudAVLRecherche* rotationDroiteRecherche(NoeudAVLRecherche* y);
NoeudAVLRecherche* insererAVLRecherche(NoeudAVLRecherche* racine, char* identifiant, NoeudDistribution* noeud);
NoeudDistribution* rechercherNoeud(NoeudAVLRecherche* racine, char* identifiant);
void libererArbreDistribution(NoeudDistribution* racine);

#endif
