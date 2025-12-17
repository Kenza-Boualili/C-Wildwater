#ifndef LECTURE_CSV_H
#define LECTURE_CSV_H

#include "structures.h"
#include "avl_usines.h" 
#include "avl_recherche.h"

/**
 * Structure temporaire utilisée pour le parsing d'une ligne du CSV.
 */
typedef struct {
    char* usine_traitement;
    char* id_amont;
    char* id_aval;  
    double volume;
    double pourcentage_fuite;
} LigneCSV;

/**
 * Détermine le type de nœud (Source, Usine, etc.) à partir de son identifiant.
 * Optimisé pour utiliser strncmp au lieu de strstr.
 */
TypeNoeud deduireType(char* id);

/**
 * Alloue et initialise un nouveau nœud de distribution.
 */
NoeudDistribution* creerNoeudDistribution(char* id);

/**
 * Ajoute un nœud enfant à un nœud parent et gère la réallocation dynamique du tableau.
 */
void ajouterEnfant(NoeudDistribution* parent, NoeudDistribution* enfant);

/**
 * Charge l'intégralité du fichier CSV et construit les structures de données (AVL).
 * Cette version est optimisée pour un parsing rapide sans allocations inutiles.
 */
int charger_csv(char* nom_fichier, NoeudAVLUsine** avl_usines, NoeudAVLRecherche** avl_recherche);

#endif
