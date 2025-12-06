#include "avl_usines.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

NoeudAVLUsine* creerNoeudAVLUsine(DonneesUsine* donnees) {
    NoeudAVLUsine* nouveau =malloc(sizeof(NoeudAVLUsine));
    if (nouveau == NULL) {
        return NULL;  
    }
    nouveau->donnees = donnees;     
    nouveau->gauche = NULL;         
    nouveau->droit = NULL;          
    nouveau->hauteur = 1;           
    return nouveau;
}


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
    if (noeud == NULL) {
        return 0;
    }
    return noeud->hauteur;
}


int equilibreAVLUsine(NoeudAVLUsine* noeud) {
    if (noeud == NULL) {
        return 0;
    }
    return hauteurAVLUsine(noeud->droit) - hauteurAVLUsine(noeud->gauche);
}


void mettreAJourHauteur(NoeudAVLUsine* noeud) {
    if (noeud != NULL) {
        int hauteurGauche = hauteurAVLUsine(noeud->gauche);
        int hauteurDroit = hauteurAVLUsine(noeud->droit);
        noeud->hauteur = 1 + max(hauteurGauche, hauteurDroit);
    }
}


NoeudAVLUsine* rotationGaucheUsine(NoeudAVLUsine* x) {
    if (x == NULL) {
        return NULL;
    }
    NoeudAVLUsine* y = x->droit;
    if (y == NULL) {
        return x;
    }
    NoeudAVLUsine* T2 = y->gauche;
    y->gauche = x;
    x->droit = T2;
    mettreAJourHauteur(x);
    mettreAJourHauteur(y);
    return y;  
}


NoeudAVLUsine* rotationDroiteUsine(NoeudAVLUsine* y) {
    if (y == NULL) {
        return NULL;
    }
    NoeudAVLUsine* x = y->gauche;
    if (x == NULL) {
        return y;
    }
    
    NoeudAVLUsine* T2 = x->droit;
    x->droit = y;
    y->gauche = T2;
    mettreAJourHauteur(y);
    mettreAJourHauteur(x);
    return x; 
}


NoeudAVLUsine* equilibrerAVLUsine(NoeudAVLUsine* racine) {
    if (racine == NULL) {
        return NULL;
    }
    mettreAJourHauteur(racine);
    int equilibre = equilibreAVLUsine(racine);
    if (equilibre < -1 && equilibreAVLUsine(racine->gauche) <= 0) {
        return rotationDroiteUsine(racine);
    }
    if (equilibre > 1 && equilibreAVLUsine(racine->droit) >= 0) {
        return rotationGaucheUsine(racine);
    }
    if (equilibre < -1 && equilibreAVLUsine(racine->gauche) > 0) {
        racine->gauche = rotationGaucheUsine(racine->gauche);
        return rotationDroiteUsine(racine);
    }
    if (equilibre > 1 && equilibreAVLUsine(racine->droit) < 0) {
        racine->droit = rotationDroiteUsine(racine->droit);
        return rotationGaucheUsine(racine);
    }
    return racine; 
}


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
        racine->donnees->capacite_max += donnees->capacite_max;
        racine->donnees->total_capte += donnees->total_capte;
        racine->donnees->total_traite += donnees->total_traite;
        free(donnees->identifiant);
        free(donnees);
        return racine; 
    }
    return equilibrerAVLUsine(racine);
}


DonneesUsine* rechercherUsine(NoeudAVLUsine* racine, const char* identifiant) {
    if (racine == NULL) {
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


void parcoursInverseAVLUsine(NoeudAVLUsine* racine, FILE* fichier, int type_histo) {
    if (racine == NULL) {
        return; 
    }
    parcoursInverseAVLUsine(racine->droit, fichier, type_histo);
    if (type_histo == 0) {
        fprintf(fichier, "%s;%.2f\n", racine->donnees->identifiant, racine->donnees->capacite_max);
    } else if (type_histo == 1) {
        fprintf(fichier, "%s;%.2f\n", racine->donnees->identifiant, racine->donnees->total_capte);
    } else if (type_histo == 2) {
        fprintf(fichier, "%s;%.2f\n", racine->donnees->identifiant, racine->donnees->total_traite);
    }
  parcoursInverseAVLUsine(racine->gauche, fichier, type_histo);
}
