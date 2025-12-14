#ifndef STRUCTURES_H
#define STRUCTURES_H

//Pour les histogrammes

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


//Pour l'arbre de distribution
typedef enum {
    NOEUD_SOURCE,
    NOEUD_USINE,        
    NOEUD_STOCKAGE,     
    NOEUD_JONCTION,     
    NOEUD_SERVICE,      
    NOEUD_CLIENT       
} TypeNoeud;

typedef struct NoeudDistribution {
    char* identifiant;                         
    TypeNoeud type;                             
                      
    double volume_entrant; 
    double pourcentage_fuite;      
    double volume_fuite;   
    double volume_sortant;                
    
    struct NoeudDistribution* parent;           
    struct NoeudDistribution** enfants;         
    int nb_enfants;                            
    int capacite_enfants;                      
} NoeudDistribution;

//Pour retrouver les noeuds rapidemment

typedef struct NoeudAVLRecherche {
    char* identifiant;                          
    NoeudDistribution* pointeur_noeud; 
    struct NoeudAVLRecherche* gauche;          
    struct NoeudAVLRecherche* droit;            
    int hauteur;                               
} NoeudAVLRecherche;

#endif
