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
    if (noeud == NULL) return 0;
    return noeud->hauteur;
}

int equilibreAVLUsine(NoeudAVLUsine* noeud) {
    if (noeud == NULL) return 0;
    return hauteurAVLUsine(noeud->droit) - hauteurAVLUsine(noeud->gauche);
}

// --- Fonctions de rotation pour l'équilibrage ---

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
    if (racine == NULL) return creerNoeudAVLUsine(donnees);
    
    int comparaison = comparerChaines(donnees->identifiant, racine->donnees->identifiant);
    if (comparaison < 0) {
        racine->gauche = insererAVLUsine(racine->gauche, donnees);
    } else if (comparaison > 0) {
        racine->droit = insererAVLUsine(racine->droit, donnees);
    } else {
        return racine; // Doublon ignoré
    }
    
    racine->hauteur = 1 + max(hauteurAVLUsine(racine->gauche), hauteurAVLUsine(racine->droit));
    return equilibrerAVLUsine(racine);
}

DonneesUsine* rechercherUsine(NoeudAVLUsine* racine, char* identifiant) {
    if (racine == NULL) return NULL; 
    
    int comparaison = comparerChaines(identifiant, racine->donnees->identifiant);
    if (comparaison == 0) {
        return racine->donnees; 
    } else if (comparaison < 0) {
        return rechercherUsine(racine->gauche, identifiant);
    } else {
        return rechercherUsine(racine->droit, identifiant);
    }
}

oid parcoursInverseAVLUsine(NoeudAVLUsine* racine, FILE* fichier, int type_histo) {
    if (racine == NULL) {
        return; 
    }
    
    // 1. On va d'abord au maximum à DROITE (identifiants alphabétiquement les plus élevés) 
    parcoursInverseAVLUsine(racine->droit, fichier, type_histo);
    
    // 2. Traitement de la racine actuelle 
    if (type_histo == 0) {
        fprintf(fichier, "%s;%.2f\n", racine->donnees->identifiant, racine->donnees->capacite_max/1000.0);
    } else if (type_histo == 1) {
        fprintf(fichier, "%s;%.2f\n", racine->donnees->identifiant, racine->donnees->total_capte/1000.0);
    } else if (type_histo == 2) {
        fprintf(fichier, "%s;%.2f\n", racine->donnees->identifiant, racine->donnees->total_traite/1000.0);
    } else if (type_histo == 3) { 
        fprintf(fichier, "%s;%.2f;%.2f;%.2f\n", 
                racine->donnees->identifiant, 
                racine->donnees->capacite_max/1000.0, 
                racine->donnees->total_capte/1000.0, 
                racine->donnees->total_traite/1000.0);
    }
    
    // 3. On termine par la GAUCHE (identifiants alphabétiquement les plus bas) 
    parcoursInverseAVLUsine(racine->gauche, fichier, type_histo);
}
