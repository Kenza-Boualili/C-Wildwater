#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct NoeudDistribution NoeudDistribution;

//Histogrammmes des usines
typedef struct DonneesUsine {
    char* identifiant;
    double capacite_max;
    double total_capte;
    double total_traite;
} DonneesUsine;

typedef struct NoeudAVLUsine {
    DonneesUsine* donnees;
    struct NoeudAVLUsine* gauche;
    struct NoeudAVLUsine* droit;
    int hauteur;
} NoeudAVLUsine;

//arbre de distribution d'eau
typedef enum {
    NOEUD_SOURCE,
    NOEUD_USINE,
    NOEUD_STOCKAGE,
    NOEUD_JONCTION,
    NOEUD_SERVICE,
    NOEUD_CLIENT
} TypeNoeud;

//lien entre les deux noeuds
typedef struct {
    NoeudDistribution* enfant;
    double pourcentage_fuite;
} LienDistribution;

//definition du noeud en entieer
struct NoeudDistribution {
    char* identifiant;
    TypeNoeud type;

    double volume_entrant;
    double pourcentage_fuite;
    double volume_fuite;
    double volume_sortant;

    struct NoeudDistribution* parent;
    LienDistribution* enfants;
    int nb_enfants;
    int capacite_enfants;
};

//avl de recherche des noeuds de distribution
typedef struct NoeudAVLRecherche {
    char* identifiant;
    NoeudDistribution* pointeur_noeud;
    struct NoeudAVLRecherche* gauche;
    struct NoeudAVLRecherche* droit;
    int hauteur;
} NoeudAVLRecherche;

#endif

