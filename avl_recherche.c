#include "avl_recherche.h"
#include <stdlib.h>
#include "utils.h"

NoeudAVLRecherche* creerNoeudAVLRecherche(char* identifiant, NoeudDistribution* noeud) {
    NoeudAVLRecherche* nouveau =malloc(sizeof(NoeudAVLRecherche));
    if (nouveau == NULL) {
        return NULL;
    }
    
    nouveau->identifiant = dupliquerChaine(identifiant);
    if (nouveau->identifiant == NULL) {
        free(nouveau);
        return NULL;
    }
    
    nouveau->pointeur_noeud = noeud; 
    nouveau->gauche = NULL;
    nouveau->droit = NULL;
    nouveau->hauteur = 1; 
    
    return nouveau;
}


void libererAVLRecherche(NoeudAVLRecherche* racine) {
    if (racine == NULL) {
        return;
    }
    libererAVLRecherche(racine->gauche);
    libererAVLRecherche(racine->droit);
    
    free(racine->identifiant);
    free(racine);
}


int hauteurAVLRecherche(NoeudAVLRecherche* noeud) {
    if (noeud == NULL) {
        return 0;
    }
    return noeud->hauteur;
}


NoeudAVLRecherche* rotationGaucheRecherche(NoeudAVLRecherche* x) {
    if (x == NULL) {
        return NULL;
    }
    
    NoeudAVLRecherche* y = x->droit;
    if (y == NULL) {
        return x;
    }
    
    NoeudAVLRecherche* T2 = y->gauche;
    
    y->gauche = x;
    x->droit = T2;
    
    x->hauteur = 1 + max(hauteurAVLRecherche(x->gauche), hauteurAVLRecherche(x->droit));
    y->hauteur = 1 + max(hauteurAVLRecherche(y->gauche), hauteurAVLRecherche(y->droit));
    
    return y; 
}


NoeudAVLRecherche* rotationDroiteRecherche(NoeudAVLRecherche* y) {
    if (y == NULL) {
        return NULL;
    }
    
    NoeudAVLRecherche* x = y->gauche;
    if (x == NULL) {
        return y;
    }
    
    NoeudAVLRecherche* T2 = x->droit;
    
    x->droit = y;
    y->gauche = T2;
    y->hauteur = 1 + max(hauteurAVLRecherche(y->gauche), hauteurAVLRecherche(y->droit));
    x->hauteur = 1 + max(hauteurAVLRecherche(x->gauche), hauteurAVLRecherche(x->droit));
    
    return x; 
}


NoeudAVLRecherche* equilibrerAVLRecherche(NoeudAVLRecherche* racine) {
    if (racine == NULL) {
        return NULL;
    }
    racine->hauteur = 1 + max(hauteurAVLRecherche(racine->gauche), 
                             hauteurAVLRecherche(racine->droit));
                             
 
    int equilibre = hauteurAVLRecherche(racine->droit) - hauteurAVLRecherche(racine->gauche);
    
    if (equilibre > 1) {
        if (hauteurAVLRecherche(racine->droit->droit) >= hauteurAVLRecherche(racine->droit->gauche)) {
            return rotationGaucheRecherche(racine); 
        } else {
            racine->droit = rotationDroiteRecherche(racine->droit); 
            return rotationGaucheRecherche(racine);
        }
    }
    
    if (equilibre < -1) {
        if (hauteurAVLRecherche(racine->gauche->gauche) >= hauteurAVLRecherche(racine->gauche->droit)) {
            return rotationDroiteRecherche(racine); 
        } else {
            racine->gauche = rotationGaucheRecherche(racine->gauche); 
            return rotationDroiteRecherche(racine);
        }
    }
    racine->hauteur = 1 + max(hauteurAVLRecherche(racine->gauche), hauteurAVLRecherche(racine->droit));
    return racine;
}


NoeudAVLRecherche* insererAVLRecherche(NoeudAVLRecherche* racine, char* identifiant, NoeudDistribution* noeud) {
    if (racine == NULL) {
        return creerNoeudAVLRecherche(identifiant, noeud); 
    }
  
    int comparaison = comparerChaines(identifiant, racine->identifiant);
    
    if (comparaison < 0) {
        racine->gauche = insererAVLRecherche(racine->gauche, identifiant, noeud); 
    } else if (comparaison > 0) {
        racine->droit = insererAVLRecherche(racine->droit, identifiant, noeud); 
    } else {
        racine->pointeur_noeud = noeud; 
        return racine;
    }
    return equilibrerAVLRecherche(racine);
}


NoeudDistribution* rechercherNoeud(NoeudAVLRecherche* racine, char* identifiant) {
    if (racine == NULL) {
        return NULL; 
    }
    
    int comparaison = comparerChaines(identifiant, racine->identifiant);
    
    if (comparaison == 0) {
        return racine->pointeur_noeud; 
    } else if (comparaison < 0) {
        return rechercherNoeud(racine->gauche, identifiant); 
    } else {
        return rechercherNoeud(racine->droit, identifiant); 
    }
}
