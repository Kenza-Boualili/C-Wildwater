#ifndef LECTURE_CSV_H
#define LECTURE_CSV_H
#include "structures.h"
#include "avl_usines.h" 
#include "avl_recherche.h"

typedef struct {
    char* usine_traitement;
    char* id_amont;
    char* id_aval;  
    double volume;
    double pourcentage_fuite;
} LigneCSV;

int contientMot(char* chaine, char* sous_chaine);
TypeNoeud deduireType(char* id);
NoeudDistribution* creerNoeudDistribution(char* id);
void ajouterEnfant(NoeudDistribution* parent, NoeudDistribution* enfant);
LigneCSV* lire_ligne_csv(char* ligne);
void liberer_ligne_csv(LigneCSV* ligne);
int charger_csv(char* nom_fichier, NoeudAVLUsine** avl_usines, NoeudAVLRecherche** avl_recherche);

#endif
