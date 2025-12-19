#ifndef LECTURE_CSV_H
#define LECTURE_CSV_H

#include "avl_usines.h"
#include "avl_recherche.h"

// Structure pour stocker une ligne CSV 
typedef struct {
    char* usine_traitement;   // Nom de l'usine de traitement
    char* id_amont;           // Identifiant du noeud amont
    char* id_aval;            // Identifiant du noeud aval
    double volume;            // Volume d'eau (k.m³/an)
    double pourcentage_fuite; // Pourcentage de fuite (%)
} LigneCSV;


LigneCSV* lire_ligne_csv(char* ligne);
void liberer_ligne_csv(LigneCSV* ligne);
NoeudDistribution* creerNoeudDistribution(char* id);
void ajouterEnfant(NoeudDistribution* parent, NoeudDistribution* enfant, double pourcentage_fuite);
NoeudDistribution* trouverRacine(NoeudAVLRecherche* avl);
TypeNoeud deduireType(char* id);
int contientMot(char* chaine, char* sous_chaine);
void verifierUsinesSansCapacite(NoeudAVLUsine* racine);
int charger_csv(FILE* fichier, NoeudAVLUsine** avl_usines, NoeudAVLRecherche** avl_recherche, char* commande);
#endif 
