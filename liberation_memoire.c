#include "liberation_memoire.h"
#include <stdlib.h>

void libererNoeudDistribution(NoeudDistribution* noeud) {
    if (noeud == NULL) {
        return;
    }
for (int i = 0; i < noeud->nb_enfants; i++) {
        libererNoeudDistribution(noeud->enfants[i]);
}
 if (noeud->enfants != NULL) {
        free(noeud->enfants);
        noeud->enfants = NULL;
 }
 if (noeud->identifiant != NULL) {
        free(noeud->identifiant);
        noeud->identifiant = NULL;
 }
 free(noeud);
}

void libererAVLRechercheComplet(NoeudAVLRecherche* racine) {
    if (racine == NULL) {
        return;
    }
libererAVLRechercheComplet(racine->gauche);
libererAVLRechercheComplet(racine->droit);
if (racine->pointeur_noeud != NULL && racine->pointeur_noeud->parent == NULL) {
        libererNoeudDistribution(racine->pointeur_noeud);
}
if (racine->identifiant != NULL) {
        free(racine->identifiant);
}
 free(racine);
}






