#include "avl_usines.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

// Crée un nouveau noeud pour l'AVL des usines
NoeudAVLUsine* creerNoeudAVLUsine(DonneesUsine* donnees) {
    NoeudAVLUsine* nouveau = malloc(sizeof(NoeudAVLUsine));
    if (nouveau == NULL) {
        return NULL;  
    }
    nouveau->donnees = donnees;     
    nouveau->gauche = NULL;         
    nouveau->droit = NULL;          
    nouveau->hauteur = 1;           
    return nouveau;
}

// Libère récursivement l'AVL et les données associées
void libererAVLUsine(NoeudAVLUsine* racine) {
    if (racine == NULL) {
        return; 
    }
    libererAVLUsine(racine->gauche);
    libererAVLUsine(racine->droit);
    if (racine->donnees != NULL) {
        if (racine->donnees->identifiant != NULL) {
            free(racine->donnees->identifiant); 
        }
        free(racine->donnees);                  
    }
    free(racine);  
}

int hauteurAVLUsine(NoeudAVLUsine* noeud) {
    if (noeud == NULL){
        return 0;
    }
    return noeud->hauteur;
}

int equilibreAVLUsine(NoeudAVLUsine* noeud) {
    if (noeud == NULL){
        return 0;
    }
    return hauteurAVLUsine(noeud->droit) - hauteurAVLUsine(noeud->gauche);
}

//Fonctions de rotation pour l'équilibrage 
NoeudAVLUsine* rotationGaucheUsine(NoeudAVLUsine* racine) {
    NoeudAVLUsine* nouveau = racine->droit;
    racine->droit = nouveau->gauche;
    nouveau->gauche = racine;
    racine->hauteur = 1 + max(hauteurAVLUsine(racine->gauche), hauteurAVLUsine(racine->droit));
    nouveau->hauteur = 1 + max(hauteurAVLUsine(nouveau->gauche), hauteurAVLUsine(nouveau->droit));
    return nouveau;
}

NoeudAVLUsine* rotationDroiteUsine(NoeudAVLUsine* racine) {
    NoeudAVLUsine* nouveau = racine->gauche;
    racine->gauche = nouveau->droit;
    nouveau->droit = racine;
    racine->hauteur = 1 + max(hauteurAVLUsine(racine->gauche), hauteurAVLUsine(racine->droit));
    nouveau->hauteur = 1 + max(hauteurAVLUsine(nouveau->gauche), hauteurAVLUsine(nouveau->droit));
    return nouveau;
}

NoeudAVLUsine* equilibrerAVLUsine(NoeudAVLUsine* racine) {
    int eq = equilibreAVLUsine(racine);
    if (eq > 1) {
        if (equilibreAVLUsine(racine->droit) < 0) {
            racine->droit = rotationDroiteUsine(racine->droit);
        }
        return rotationGaucheUsine(racine);
    }
    if (eq < -1) {
        if (equilibreAVLUsine(racine->gauche) > 0) {
            racine->gauche = rotationGaucheUsine(racine->gauche);
        }
        return rotationDroiteUsine(racine);
    }
    return racine;
}

// Insertion avec maintien de l'équilibre
NoeudAVLUsine* insererAVLUsine(NoeudAVLUsine* racine, DonneesUsine* donnees) {
    if (racine == NULL) {
        return creerNoeudAVLUsine(donnees);
    }
    
    int comparaison = comparerChaines(donnees->identifiant, racine->donnees->identifiant);
    if (comparaison < 0) {
        racine->gauche = insererAVLUsine(racine->gauche, donnees);
    } else if (comparaison > 0) {
        racine->droit = insererAVLUsine(racine->droit, donnees);
    } else {
        // Mise à jour des données si l'usine existe déjà
        racine->donnees->total_capte += donnees->total_capte;
        racine->donnees->total_traite += donnees->total_traite;
        free(donnees->identifiant);
        free(donnees);
        return racine; 
    }
    
    racine->hauteur = 1 + max(hauteurAVLUsine(racine->gauche), hauteurAVLUsine(racine->droit));
    return equilibrerAVLUsine(racine);
}

DonneesUsine* rechercherUsine(NoeudAVLUsine* racine, char* identifiant) {
    if (racine == NULL){
    return NULL; 
    } 
    int comparaison = comparerChaines(identifiant, racine->donnees->identifiant);
    if (comparaison == 0) {
        return racine->donnees; 
    } else if (comparaison < 0) {
        return rechercherUsine(racine->gauche, identifiant);
    } else {
        return rechercherUsine(racine->droit, identifiant);
    }
}

// Fonction pour ajuster (plafonner) les volumes traités par les usines
// selon leur capacité maximale théorique
void plafonnerVolumesUsines(NoeudAVLUsine* racine) {
    if (racine == NULL){
    return;
    } 
    plafonnerVolumesUsines(racine->gauche);
    
    // Si le calcul dépasse la capacité de l'usine, on plafonne
    if (racine->donnees->capacite_max > 0 && racine->donnees->total_traite > racine->donnees->capacite_max) {
        racine->donnees->total_traite = racine->donnees->capacite_max;
    }
    
    plafonnerVolumesUsines(racine->droit);
}

void parcoursInverseAVLUsine(NoeudAVLUsine* racine, FILE* fichier, int type_histo) {
    if (racine == NULL){
    return; 
    } 
    // On parcourt de Droite à Gauche pour avoir un tri alphabétique Z -> A
    parcoursInverseAVLUsine(racine->droit, fichier, type_histo);
    
    double valeur = 0.0;
    if (type_histo == 0){
        valeur = racine->donnees->capacite_max;
    }
    else if (type_histo == 1){
        valeur = racine->donnees->total_capte;
    }
    else if (type_histo == 2){
        valeur = racine->donnees->total_traite;
}

    // Écriture : ID ; Valeur (Real/Src) ; Capacité 
    fprintf(fichier, "%s;%.2f;%.2f\n", racine->donnees->identifiant, valeur, racine->donnees->capacite_max);
    parcoursInverseAVLUsine(racine->gauche, fichier, type_histo);
}
