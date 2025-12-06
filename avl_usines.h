#ifndef AVL_USINES_H
#define AVL_USINES_H
#include "structures.h"
#include <stdio.h>
#include "utils.h"

NoeudAVLUsine* creerNoeudAVLUsine(DonneesUsine* donnees);
void libererAVLUsine(NoeudAVLUsine* racine);
int hauteurAVLUsine(NoeudAVLUsine* noeud);
int equilibreAVLUsine(NoeudAVLUsine* noeud);
NoeudAVLUsine* equilibrerAVLUsine(NoeudAVLUsine* racine);
NoeudAVLUsine* rotationGaucheUsine(NoeudAVLUsine* racine);
NoeudAVLUsine* rotationDroiteUsine(NoeudAVLUsine* racine);
NoeudAVLUsine* insererAVLUsine(NoeudAVLUsine* racine, DonneesUsine* donnees);
DonneesUsine* rechercherUsine(NoeudAVLUsine* racine, char* identifiant);
void parcoursInverseAVLUsine(NoeudAVLUsine* racine, FILE* fichier, int type_histo);

#endif
