#ifndef AVL_RECHERCHE_H
#define AVL_RECHERCHE_H
#include "structures.h"
#include "utils.h"

NoeudAVLRecherche* creerNoeudAVLRecherche(char* identifiant, NoeudDistribution* noeud);
void libererAVLRecherche(NoeudAVLRecherche* racine);
int hauteurAVLRecherche(NoeudAVLRecherche* noeud);
NoeudAVLRecherche* equilibrerAVLRecherche(NoeudAVLRecherche* racine);
NoeudAVLRecherche* rotationGaucheRecherche(NoeudAVLRecherche* racine);
NoeudAVLRecherche* rotationDroiteRecherche(NoeudAVLRecherche* racine);
NoeudAVLRecherche* insererAVLRecherche(NoeudAVLRecherche* racine, char* identifiant, NoeudDistribution* noeud);
NoeudDistribution* rechercherNoeud(NoeudAVLRecherche* racine, char* identifiant);
void libererArbreDistribution(NoeudDistribution* racine);

#endif
