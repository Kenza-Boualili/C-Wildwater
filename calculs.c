#include "calculs.h"
#include <stdlib.h> 

double calculerFuites(NoeudDistribution* noeud, double volume_arrivee) {
    if (noeud == NULL){
        return 0;
    } 
    double fuite_ici = 0;
    if (noeud->pourcentage_fuite > 0) {
        fuite_ici = volume_arrivee * (noeud->pourcentage_fuite / 100.0);
    }
    double volume_restant = volume_arrivee - fuite_ici;
    double volume_par_enfant = 0;
    if (noeud->nb_enfants > 0) {
        volume_par_enfant = volume_restant / noeud->nb_enfants;
    }
    double total_fuites_enfants = 0;
    for (int i = 0; i < noeud->nb_enfants; i++) {
        total_fuites_enfants += calculerFuites(noeud->enfants[i], volume_par_enfant);
    }
    return fuite_ici + total_fuites_enfants;
}
